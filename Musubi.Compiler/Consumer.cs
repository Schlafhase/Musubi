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
            return EndReached() ?  EndOfSourceValue : Source[_current++] ;
        }

        protected T Peek(int offset = 0)
        {
            if (_current + offset >= Source.Count || _current + offset < 0)
            {
                return EndOfSourceValue;
            }
            return Source[_current + offset];
        }

        protected void SkipNextIf(Func<T, bool> predicate)
        {
            if (predicate(Peek()))
            {
                Advance();
            }
        }
    }
}