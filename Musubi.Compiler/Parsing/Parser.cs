using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing
{
    public class Parser(List<Token> tokens, Errors errors)
    {
        private int _current;
        private readonly Stack<string> _knownVariables = [];
        private readonly List<string> _definedAliases = [];

        public Node Parse()
        {
            Node root = document();
            if (!endReached())
            {
                errors.ReportUnexpected(peek(), "end of file");
            }
            return root;
        }

        // document: definition* expression
        private Document document()
        {
            List<Alias> definitions = [];
            while (checkNext(TokenType.Identifier, TokenType.Definition))
            {
                definitions.Add(definition());
            }
            Node toplevel = expression();
            expect("a semicolon", TokenType.StatementEnd);
            return new() { Definitions = definitions, Expression = toplevel };
        }

        private Alias definition()
        {
            string name = identifier();
            if (_definedAliases.Contains(name))
            {
                errors.ReportToken(previous(), $"Duplicate definition of alias '{name}'");
            }
            expect(TokenType.Definition);
            Node value = expression();
            expect("a semicolon", TokenType.StatementEnd);
            _definedAliases.Add(name);
            return new() { Name = name, Value = value };
        }

        // expression: atom atom*
        private Node expression()
        {
            Node left = atom();

            while (
                check(
                    TokenType.Lambda,
                    TokenType.Identifier,
                    TokenType.LeftParen,
                    TokenType.True,
                    TokenType.False,
                    TokenType.Number
                )
            )
            {
                Node right = atom();
                left = new Application() { Function = left, Argument = right };
            }

            return left;
        }

        // atom: lambda | variable | "(" expression ")" | true | false
        private Node atom()
        {
            switch (peek().Type)
            {
                case TokenType.Lambda:
                    return lambda();
                case TokenType.Identifier:
                    string id = identifier();
                    if (_knownVariables.Contains(id))
                    {
                        return new Variable() { Name = id };
                    }
                    else if (_definedAliases.Contains(id))
                    {
                        return new AliasReference() { Alias = id };
                    }
                    errors.ReportToken(previous(), $"Undefined identifier '{id}'");
                    return new Error();
                case TokenType.LeftParen:
                    advance();
                    Node inner = expression();
                    expect("closing parenthesis", TokenType.RightParen);
                    return inner;
                case TokenType.Number:
                    advance();
                    return new Number() { Value = int.Parse(previous().Lexeme) };
                case TokenType.True:
                    advance();
                    return new True();
                case TokenType.False:
                    advance();
                    return new False();
                default:
                    errors.ReportUnexpected(peek(), "expression");
                    return new Error();
            }
        }

        // lambda: ("\" | "λ") identifier "." expression
        private Node lambda()
        {
            if (expect(TokenType.Lambda))
            {
                string captured = identifier();
                if (expect(TokenType.Dot))
                {
                    _knownVariables.Push(captured);
                    Node body = expression();
                    _knownVariables.Pop();
                    return new Lambda() { CapturedVariable = captured, Body = body };
                }
            }
            return new Error();
        }

        private string identifier()
        {
            expect(TokenType.Identifier);
            return previous().Lexeme;
        }

        // helpers

        private void advance()
        {
            _current++;
        }

        private Token previous()
        {
            return tokens[_current - 1];
        }

        private Token peek()
        {
            return tokens[_current];
        }

        private bool check(params TokenType[] types)
        {
            if (endReached())
            {
                return false;
            }
            return types.Contains(peek().Type);
        }

        private bool checkNext(params TokenType[] typeSequence)
        {
            if (_current + typeSequence.Length >= tokens.Count)
            {
                return false;
            }
            for (int i = 0; i < typeSequence.Length; i++)
            {
                if (typeSequence[i] != tokens[_current + i].Type)
                {
                    return false;
                }
            }
            return true;
        }

        private bool match(params TokenType[] types)
        {
            if (check(types))
            {
                advance();
                return true;
            }

            return false;
        }

        private bool expect(params TokenType[] types)
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.ReportUnexpected(next, types);
            return false;
        }

        private bool expect(string expected, params TokenType[] types)
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.ReportUnexpected(next, expected);
            return false;
        }

        private bool endReached()
        {
            return peek().Type == TokenType.EOF;
        }
    }
}
