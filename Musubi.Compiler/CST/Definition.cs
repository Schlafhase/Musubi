namespace Musubi.Compiler.CST
{
    public struct InfixOptions
    {
        public required Associativity Associativity;
        public required int BindingPrecedence;
    }

    public sealed class Definition : CSTNonLeaf
    {
        public required string Name;
        public InfixOptions? InfixOptions;
    }
}
