namespace Musubi.Compiler
{
    public record CodeLocation(string Filename, int Line, int Column)
    {
        public static CodeLocation operator +(CodeLocation a, int b) =>
            a with
            {
                Column = a.Column + b,
            };

        public static CodeLocation operator -(CodeLocation a, int b) =>
            a with
            {
                Column = a.Column <= 0 ? a.Column : a.Column - b,
            };

        public static implicit operator CodeRange(CodeLocation l) => l.Extend(0);

        /// <summary>
        /// Makes a <see cref="CodeRange">CodeRange</see> out
        /// of this <see cref="CodeLocation">CodeLocation</see>
        /// by extending it by <paramref name="n"/> characters on
        /// the same line.
        /// </summary>
        public CodeRange Extend(int n)
        {
            return new CodeRange(this, this + n - 1);
        }

        public CodeRange To(CodeLocation b)
        {
            return new CodeRange(this, b);
        }

        public override string ToString()
        {
            return $"{Filename}:{Line}:{Column}";
        }
    }

    public record CodeRange(CodeLocation Start, CodeLocation End) : IComparable<CodeRange>
    {
        public int? Length => Start.Line == End.Line ? End.Column - Start.Column : null;

        public int CompareTo(CodeRange? other)
        {
            if (other is null)
            {
                return 1;
            }
            if (
                (End.Line - Start.Line) == (other.End.Line - other.Start.Line)
                && (End.Column - Start.Column) == (other.End.Column - other.Start.Column)
            )
            {
                return 0;
            }
            return
                (End.Line - Start.Line) > (other.End.Line - other.Start.Line)
                || (
                    (End.Line - Start.Line) == (other.End.Line - other.Start.Line)
                    && (End.Column - Start.Column) > (other.End.Column - other.Start.Column)
                )
                ? 1
                : -1;
        }

        public override string ToString()
        {
            return $"{Start} - {End.Line}:{End.Column}";
        }
    }
}
