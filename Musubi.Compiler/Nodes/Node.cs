namespace Musubi.Compiler.Nodes
{
    public abstract class Node
    {
        public string Id = Guid.NewGuid().ToString().Replace("-", "");
        public required CodeRange Range;
        public string? DebugIdentifier;
    }
}
