using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.CST
{
    public abstract class CSTNode
    {
        public string Id = Guid.NewGuid().ToString().Replace("-", "");
        public string? DebugIdentifier;
        public required Token Start;
        public required Token End;
    }
}
