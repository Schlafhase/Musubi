namespace Musubi.Compiler.AST
{
    public enum NumeralEncoding
    {
        Binary,
        Scott,
        Church,
    }

    public sealed class Number : Node
    {
        public required int Value;
        public NumeralEncoding Encoding;
    }
}