namespace Musubi.Compiler
{
    public record CodeLocation(string Filename, int Line, int Column)
    {
        public override string ToString()
        {
            return $"{Filename}:{Line}:{Column}";
        }
    }

    public record CodeRange(CodeLocation A, CodeLocation B)
    {
        public override string ToString()
        {
            return $"{A} - {B.Line}:{B.Column}";
        }
    }
}