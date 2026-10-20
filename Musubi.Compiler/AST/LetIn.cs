namespace Musubi.Compiler.AST
{
    public class LetIn : Node
    {
        public required Dictionary<string, Node> Definitions;
        public required Node Expression;
    }
}