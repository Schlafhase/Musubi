using System.Collections.Immutable;
using Musubi.Compiler.CST;
using Musubi.Compiler.Expanding;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing;

public readonly struct ParserOut
{
    public required CSTNode CST { get; init; }

    public required CompilationContext Context { get; init; }
}

public static class ExpanderOutParserExtension
{
    public static ParserOut Parse(this ExpanderOut input)
    {
        Parser p = new(input);
        return p.Parse();
    }
}

public class Parser(ExpanderOut input) : Consumer<Token>
{
    protected override List<Token> Source => input.Tokens;
    protected override Token EndOfSourceValue => Source[^1]; // should be the EOF token

    private ErrorLogger errorLogger => input.Context.ErrorLogger;
    private int _start = 0;

    private Token start => Source[_start];
    private Token end => Source[_current - 1];
    private int startIndex => _start;
    private int endIndex => _current - 1;

    public ParserOut Parse()
    {
        CSTNode root = document();
        if (!EndReached())
        {
            errorLogger.AddDiagnostic(
                Peek().Range,
                Errors.UnexpectedToken,
                TokenType.EOF,
                Peek().Type
            );
        }
        return new ParserOut() { Context = input.Context, CST = root };
    }

    private Document document()
    {
        _start = _current;
        CSTNode toplevel = expression();

        Document doc = initNode<Document>();
        doc.Children = [toplevel];
        return doc;
    }

    private CSTNode expression(int minBindingPrecedence = 0)
    {
        CSTNode left = atom();

        while (
            check(
                TokenType.Lambda,
                TokenType.Identifier,
                TokenType.LeftParen,
                TokenType.Number,
                TokenType.ScottNumber,
                TokenType.ChurchNumber,
                TokenType.BinaryNumber,
                TokenType.String,
                TokenType.Char,
                TokenType.Let,
                TokenType.BeginMacro,
                TokenType.BeginInclude
            )
        )
            ;
    }

    private CSTNode atom()
    {
        TokenType type = Peek().Type;
        switch (type)
        {
            case TokenType.Lambda:
                return lambda();
            case TokenType.LeftParen:
                return parens();
            case TokenType.Identifier:
                Advance();
                Identifier id = initNode<Identifier>();
                id.Name = (string)Previous().Literal!;
                return id;
            case TokenType.ChurchNumber:
            case TokenType.ScottNumber:
            case TokenType.BinaryNumber:
            case TokenType.Number:
                Advance();
                NumberLiteral nl = initNode<NumberLiteral>();
                nl.Value = (int)Previous().Literal!;
                nl.Encoding = type switch
                {
                    TokenType.ChurchNumber => NumeralEncoding.Church,
                    TokenType.ScottNumber => NumeralEncoding.Scott,
                    TokenType.BinaryNumber => NumeralEncoding.Binary,
                    _ => NumeralEncoding.Scott,
                };
                return nl;
            case TokenType.Let:
                return letin();
            default:
                errorLogger.AddDiagnostic(Peek().Range, Errors.ExpectedExpression, Peek().Type);
                return initNode<SyntaxError>();
        }
    }

    private bool check(params TokenType[] types)
    {
        if (EndReached())
        {
            return types.Contains(TokenType.EOF);
        }
        return types.Contains(Peek().Type);
    }

    private T initNode<T>()
        where T : CSTNode
    {
        T ret = Activator.CreateInstance<T>();
        ret.Start = start;
        ret.StartIndex = startIndex;
        ret.End = end;
        ret.EndIndex = endIndex;
        return ret;
    }
}
