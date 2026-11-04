namespace Musubi.Compiler.CST
{
    public sealed class NumberLiteral : Literal<int>
    {
        public required NumeralEncoding Encoding;
    }
}
