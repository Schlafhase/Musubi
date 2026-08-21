namespace Musubi.Compiler.Nodes
{
    public sealed class Lambda : Node
    {
        public required string CapturedVariable;
        public required Node Body;
    }
}