using System.Text;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.CST
{
    public abstract class CSTNode
    {
        public string Id = Guid.NewGuid().ToString().Replace("-", "");
        public string? DebugIdentifier;

        public required Token Start;
        public required int StartIndex;
        public required Token End;
        public required int EndIndex;

        public string GetStringRepresentation()
        {
            StringBuilder sb = new();
            sb.Append(GetType().Name);
            if (this is ICSTDataNode dataNode)
            {
                sb.Append($"[{dataNode.BoxedData}]");
            }
            if (this is CSTNonLeaf nonLeaf)
            {
                sb.Append('(');
                foreach (CSTNode child in nonLeaf.Children)
                {
                    sb.Append(child.GetStringRepresentation());
                }
                sb.Append(')');
            }
            return sb.ToString();
        }
    }
}