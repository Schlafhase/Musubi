namespace Musubi.Compiler
{
    public abstract class Consumer<T>
    {
        protected abstract List<T> Source { get; }
        protected abstract T EndOfSourceValue { get; }
        protected int _current;

        protected bool EndReached()
        {
            return _current >= Source.Count;
        }

        protected T Advance()
        {
            return EndReached() ? Source[_current++] : EndOfSourceValue;
        }

        protected T Peek(int offset = 0)
        {
            if (_current + offset >= Source.Count)
            {
                return EndOfSourceValue;
            }
            return Source[_current + offset];
        }
    }
}
