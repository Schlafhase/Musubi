namespace Musubi.Compiler.AST
{
    public sealed class Variable : Node
    {
        public required string ReferencedVariable;
        public Node? ReferencedDefinition;
        public required int DeBruijn;
    }
}