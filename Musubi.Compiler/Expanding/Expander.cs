using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Expanding;

public readonly struct ExpanderOut
{
    public readonly List<Token> Tokens;
    public required Errors Errors { get; init; }
}

public sealed class Expander(ScannerOut input)
{
    public ExpanderOut Expand()
    {
        throw new NotImplementedException();
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
