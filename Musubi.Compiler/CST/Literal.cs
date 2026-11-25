namespace Musubi.Compiler.CST
{
    public abstract class Literal<T> : CSTNode, ICSTDataNode<T>
    {
        public required T Value;

        public T Data => Value;
    }
}
