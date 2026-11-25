namespace Musubi.Compiler.CST
{
    public sealed class Identifier : CSTNode, ICSTDataNode<string>
    {
        public required string Name;
        public string Data => Name;
    }
}