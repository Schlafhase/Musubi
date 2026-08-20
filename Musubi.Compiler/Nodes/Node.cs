namespace Musubi.Compiler.Nodes
{
    public abstract class Node
    {
        public string Id = Guid.NewGuid().ToString().Replace("-", "");
        public CodeRange? Range;
        public string? DebugIdentifier;
    }
}
