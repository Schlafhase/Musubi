namespace Musubi.Compiler.AST
{
    public sealed class Lambda : Node
    {
        public string? VariableDocString;
        public required string CapturedVariable;
        public required Node Body;
    }
}