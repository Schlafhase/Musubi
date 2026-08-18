namespace Musubi.Compiler
{
    public record CodeLocation(string Filename, int Line, int Column)
    {
        public override string ToString()
        {
            return $"{Filename}:{Line}:{Column}";
        }
    }

    public record CodeRange(CodeLocation Location, int Length = 1);
}
