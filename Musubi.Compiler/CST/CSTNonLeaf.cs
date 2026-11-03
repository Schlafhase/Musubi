namespace Musubi.Compiler.CST
{
    public abstract class CSTNonLeaf : CSTNode
    {
        public List<CSTNode> Children = [];
    }
}
