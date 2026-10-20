namespace Musubi.Compiler
{
    public record CodeLocation(string Filename, int Line, int Column)
    {
        public override string ToString()
        {
            return $"{Filename}:{Line}:{Column}";
        }
    }

    public record CodeRange(CodeLocation A, CodeLocation B) : IComparable<CodeRange>
    {
        public int CompareTo(CodeRange? other)
        {
            if (other is null)
            {
                return 1;
            }
            if ((B.Line - A.Line) == (other.B.Line - other.A.Line) && (B.Column - A.Column) == (other.B.Column - other.A.Column))
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
