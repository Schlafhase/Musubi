namespace Musubi.Compiler.AST
{
    public abstract class Node
    {
        public string Id = Guid.NewGuid().ToString().Replace("-", "");
        public string? DebugIdentifier;
        public string? DocString;
    }
}