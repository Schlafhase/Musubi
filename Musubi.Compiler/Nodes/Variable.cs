namespace Musubi.Compiler.Nodes
{
    public sealed class Variable : Node
    {
        public required string ReferencedVariable;
        public required int DeBruijn;
    }
}