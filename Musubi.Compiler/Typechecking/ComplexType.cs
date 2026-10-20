namespace Musubi.Compiler.Typechecking
{
    public class ComplexType : IType
    {
        public required List<IType> TypeParameters;

        public required IType Parameter;
        public required IType Return;
    }
}
