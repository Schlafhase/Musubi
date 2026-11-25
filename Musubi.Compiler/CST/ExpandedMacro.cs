namespace Musubi.Compiler.CST
{
    public class ExpandedMacro : CSTNonLeaf, ICSTDataNode<string> { 
        public required string MacroName;
        public string Data => MacroName;
    }
}
