namespace Musubi.Compiler.Nodes
{
    public sealed class Document : Node {
        public required List<Alias> Definitions;
        public required Node Expression;
    }
}
