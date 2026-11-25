using System.Collections.Immutable;
using Musubi.Compiler.Exceptions;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Expanding;

public readonly struct ExpanderOut
{
    public readonly List<Token> Tokens = [];

    public required CompilationContext Context { get; init; }

    public ExpanderOut() { }
}

public delegate string FileResolver(string target, string from, out string resolvedTo);

public static class ScannerOutExpandExtension
{
    public static ExpanderOut Expand(
        this ScannerOut input,
        FileResolver? fileResolver = null,
        ImmutableHashSet<string>? includedFrom = null,
        HashSet<string>? alreadyIncluded = null,
        Dictionary<string, List<Token>>? macros = null
    )
    {
        Expander e = new(input, fileResolver, includedFrom, alreadyIncluded, macros);
        return e.Expand();
    }
}

/// <summary>
/// Expands macros and includes
/// </summary>
/// <param name="input">Output from the scanner</param>
/// <param name="fileResolver">function that takes a filepath to a target file and a filepath to the source file and returns the content of the target file</param>
public sealed class Expander(
    ScannerOut input,
    FileResolver? fileResolver = null,
    ImmutableHashSet<string>? includedFrom = null,
    HashSet<string>? alreadyIncluded = null,
    Dictionary<string, List<Token>>? macros = null
) : Consumer<Token>
{
    private readonly FileResolver _fileResolver = fileResolver ?? FileLookup.DefaultFileResolver;
    private readonly ImmutableHashSet<string> _includedFrom = includedFrom ?? [];
    private readonly HashSet<string> _alreadyIncluded = alreadyIncluded ?? [];
    private readonly HashSet<string> _alreadyIncludedInCurrentDoc = [];

    private ErrorLogger errorLogger => input.Context.ErrorLogger;

    protected override List<Token> Source => input.Tokens;

    protected override Token EndOfSourceValue => Source[^1]; // should be the EOF token

    private readonly List<Token> _expanded = [];
    private readonly Dictionary<string, List<Token>> _macros = macros ?? [];

    public ExpanderOut Expand()
    {
        if (_includedFrom.Contains(input.Context.Source.File.FullName))
        {
            throw new CircularIncludeException();
        }

        while (!EndReached())
        {
            Token t = Advance();
            handleToken(t);
        }

        ExpanderOut result = new() { Context = input.Context };
        result.Tokens.AddRange(_expanded);
        return result;
    }

    private void handleToken(Token t)
    {
        switch (t.Type)
        {
            case TokenType.Define:
                handleMacroDefinition(t);
                break;

            case TokenType.Identifier:
                handleIdentifier(t);
                break;

            case TokenType.Include:
                handleInclude(t);
                break;

            default:
                addToken(t);
                break;
        }
    }

    private void handleMacroDefinition(Token t)
    {
        addToken(t);
        if (t.LastOnLine)
        {
            errorLogger.AddDiagnostic(t.Range.End, Errors.NewlineInDirective);
            return;
        }

        Token current = Advance();
        addToken(current);

        if (current.Type != TokenType.Identifier)
        {
            errorLogger.AddDiagnostic(
                current.Range,
                Errors.UnexpectedToken,
                TokenType.Identifier,
                current.Type
            );
            return;
        }
        string id = (current.Literal as string)!;

        List<Token> content = [];

        while (!current.LastOnLine && Peek().Type != TokenType.EOF)
        {
            current = Advance();
            content.Add(current);
            addToken(current);
        }

        if (
            content.FindIndex(t => t.Type is TokenType.Define or TokenType.Include)
            is >= 0
                and int i
        )
        {
            errorLogger.AddDiagnostic(content[i].Range, Errors.TokenNotAllowedInMacro);
            // TODO: check if i should also remove those tokens from the list or if the parser can handle it
        }

        _macros[id] = content;
    }

    private void handleIdentifier(Token t)
    {
        if (_macros.TryGetValue((t.Literal as string)!, out var content))
        {
            addToken(
                new Token(TokenType.BeginMacro, t.Lexeme, t.Line, t.Column, t.Filename)
                {
                    Index = _expanded.Count,
                    Literal = t.Literal,
                    LeadingTrivia = t.LeadingTrivia,
                }
            );

            foreach (Token c in content)
            {
                addToken(c);
            }

            _expanded.Add(
                new Token(TokenType.EndMacro, "", t.Line, t.Range.End.Column, t.Filename)
                {
                    Index = _expanded.Count,
                    TrailingTrivia = t.TrailingTrivia,
                }
            );
            return;
        }

        addToken(t);
    }

    private void handleInclude(Token t)
    {
        if (t.LastOnLine)
        {
            addToken(t);
            errorLogger.AddDiagnostic(t.Range.End, Errors.NewlineInDirective);
            // skip to safe point and discard this include
            SkipNextIf(t => t.Type is TokenType.Filename);
            return;
        }

        Token targetToken = Advance();
        List<Token> moduleContent = [];
        string? target = targetToken.Literal as string;

        if (targetToken.Type != TokenType.Filename)
        {
            errorLogger.AddDiagnostic(
                targetToken.Range,
                Errors.UnexpectedToken,
                TokenType.Filename,
                targetToken.Type
            );
        }
        else
        {
            try
            {
                moduleContent = getModule(target!);
            }
            catch (FileNotFoundException)
            {
                errorLogger.AddDiagnostic(targetToken.Range, Errors.FileNotFound);
            }
            catch (CircularIncludeException)
            {
                errorLogger.AddDiagnostic(targetToken.Range, Errors.CircularInclude);
            }
            catch (DuplicateIncludeWarning)
            {
                errorLogger.AddDiagnostic(targetToken.Range, Errors.DuplicateInclude);
            }
        }

        _expanded.Add(
            new(
                TokenType.BeginInclude,
                t.Lexeme
                    + string.Concat(t.TrailingTrivia.Select(t => t.ToString()))
                    + string.Concat(targetToken.LeadingTrivia.Select(t => t.ToString()))
                    + targetToken.Lexeme,
                t.Line,
                t.Column,
                t.Filename
            )
            {
                Index = _expanded.Count,
                LeadingTrivia = t.LeadingTrivia,
                Literal = target,
            }
        );

        foreach (Token mt in moduleContent)
        {
            addToken(mt);
        }

        _expanded.Add(
            new(TokenType.EndInclude, "", t.Line, t.Column, t.Filename)
            {
                Index = _expanded.Count,
                TrailingTrivia = targetToken.TrailingTrivia,
            }
        );
    }

    private List<Token> getModule(string target)
    {
        string source = _fileResolver(target, input.Context.Source.Directory, out string fullPath);

        CompilationContext moduleContext = new(
            new MusubiSource(source, new FileInfo(fullPath)),
            errorLogger
        );

        List<Token> ret =
        [
            .. moduleContext
                .ScanTokens()
                .Expand(
                    _fileResolver,
                    _includedFrom.Add(input.Context.Source.File.FullName),
                    _alreadyIncluded,
                    _macros
                )
                .Tokens.Where(t => t.Type != TokenType.EOF),
        ];
        if (_alreadyIncluded.Contains(fullPath) && ret.Any(t => t.Type == TokenType.IncludeOnce))
        {
            if (_alreadyIncludedInCurrentDoc.Contains(fullPath))
            {
                throw new DuplicateIncludeWarning();
            }
            return [];
        }
        _alreadyIncluded.Add(fullPath);
        _alreadyIncludedInCurrentDoc.Add(fullPath);
        return ret;
    }

    private void addToken(Token t)
    {
        _expanded.Add(t with { Index = _expanded.Count });
    }
}