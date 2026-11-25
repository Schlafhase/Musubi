namespace Musubi.Compiler.CST
{
    public class InfixApplication : CSTNonLeaf, ICSTDataNode<string> { 
        public required string OperatorName;
        public string Data => OperatorName;
    }
}
