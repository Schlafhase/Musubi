namespace Musubi.Compiler.Scanning
{
    public sealed record Newline : Trivia
    {
        public override string ToString()
        {
            return "\n";
        }
    }
}