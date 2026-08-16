namespace Musubi.Compiler.Nodes
{
    public class LetIn : Node{ 
        public required Dictionary<string, Node> Definitions;
        public required Node Expression;
    }
}
