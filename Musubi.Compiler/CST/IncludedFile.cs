namespace Musubi.Compiler.CST
{
    public class IncludedFile : CSTNonLeaf, ICSTDataNode<string>
    {
        public required string File;
        public string Data => File;
    }
}
