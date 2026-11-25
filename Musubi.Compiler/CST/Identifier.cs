namespace Musubi.Compiler.CST
{
    public sealed class Identifier : CSTNonLeaf, ICSTDataNode<string>
    {
        public required string Name;
        public string Data => Name;
    }
}
