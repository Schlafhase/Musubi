namespace Musubi.Compiler.Nodes
{
    public sealed class Alias : Node{ 
        public required string Name;
        public required Node Value;
    }
}
