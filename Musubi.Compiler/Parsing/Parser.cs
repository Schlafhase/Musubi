using System.Collections.Immutable;
using Musubi.Compiler.AST;
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

public class Parser(ExpanderOut input) { }
