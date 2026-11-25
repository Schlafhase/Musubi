namespace Musubi.Compiler.CST
{
    public interface ICSTDataNode
    {
        object? BoxedData { get; }
    }

    public interface ICSTDataNode<out T> : ICSTDataNode
    {
        T Data { get; }
        object? ICSTDataNode.BoxedData
        {
            get => Data;
        }
    }
}
