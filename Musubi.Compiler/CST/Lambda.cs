namespace Musubi.Compiler.CST
{
    public sealed class Lambda : CSTNode, ICSTDataNode<string>
    {
        public required string CapturedVariable;
        public string Data => CapturedVariable;
    }
}