using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Expanding;

public readonly struct ExpanderOut
{
    public readonly List<Token> Tokens = [];

    public required ErrorLogger ErrorLogger { get; init; }

    public ExpanderOut()
    {
    }
}

public delegate string FileResolver(string target, string from);

/// <summary>
/// Expands macros and includes
/// </summary>
/// <param name="input">Output from the scanner</param>
/// <param name="fileResolver">function that takes a filepath to a target file and a filepath to the source file and returns an absolute path to the target file</param>
public sealed class Expander(ScannerOut input, FileResolver? fileResolver = null) : Consumer<Token>
{
    private FileResolver _fileResolver = fileResolver ?? FileLookup.GetPath;

    private ErrorLogger _errors => input.ErrorLogger;

    protected override List<Token> Source => input.Tokens;

    protected override Token EndOfSourceValue => Source[^1]; // should be the EOF token

    private readonly List<Token> _expanded = [];
    private readonly Dictionary<string, List<Token>> _macros = [];

    public ExpanderOut Expand()
    {
        while (!EndReached())
        {
            Token t = Advance();
            handleToken(t);
        }

        ExpanderOut result = new()
        {
            ErrorLogger = input.ErrorLogger
        };
        result.Tokens.AddRange(_expanded);
        return result;
    }

    private void handleToken(Token t)
    {
        switch (t.Type)
        {
            default:
                _expanded.Add(t);
                break;
        }
    }
}

public static class ScannerOutExpandExtension
{
    public static ExpanderOut Expand(this ScannerOut input)
    {
        Expander e = new(input);
        return e.Expand();
    }
}
