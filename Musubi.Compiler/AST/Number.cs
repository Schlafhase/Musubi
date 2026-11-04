using Musubi.Compiler.CST;

namespace Musubi.Compiler.AST
{
    public sealed class Number : Node
    {
        public required int Value;
        public NumeralEncoding Encoding;
    }
}