namespace Musubi.Compiler.Nodes
{
    public sealed class Number : Node
    {
        public required int Value;
        public bool ChurchEncoded;
    }
}
