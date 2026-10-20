namespace Musubi.Compiler.AST
{
    public sealed class Application : Node
    {
        public required Node Function;
        public required Node Argument;
    }
}