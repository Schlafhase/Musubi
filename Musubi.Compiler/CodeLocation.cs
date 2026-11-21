namespace Musubi.Compiler
{
    public record CodeLocation(string Filename, int Line, int Column)
    {
        public static CodeLocation operator +(CodeLocation a, int b) =>
            a with
            {
                Column = a.Column + b,
            };

        /// <summary>
        /// Makes a <see cref="CodeRange">CodeRange</see> out
        /// of this <see cref="CodeLocation">CodeLocation</see>
        /// by extending it by <paramref name="n"/> characters on
        /// the same line.
        /// </summary>
        public CodeRange Extend(int n)
        {
            return new CodeRange(this, this + n);
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

    public record CodeRange(CodeLocation A, CodeLocation B) : IComparable<CodeRange>
    {
        public int? Length => A.Line == B.Line ? B.Column - A.Column : null;

        public int CompareTo(CodeRange? other)
        {
            if (other is null)
            {
                return 1;
            }
            if (
                (B.Line - A.Line) == (other.B.Line - other.A.Line)
                && (B.Column - A.Column) == (other.B.Column - other.A.Column)
            )
            {
                return 0;
            }
            return
                (B.Line - A.Line) > (other.B.Line - other.A.Line)
                || (
                    (B.Line - A.Line) == (other.B.Line - other.A.Line)
                    && (B.Column - A.Column) > (other.B.Column - other.A.Column)
                )
                ? 1
                : -1;
        }

        public override string ToString()
        {
            return $"{A} - {B.Line}:{B.Column}";
        }
    }
}
