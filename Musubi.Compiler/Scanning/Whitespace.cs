namespace Musubi.Compiler.Scanning
{
    public sealed record Whitespace(string Text) : Trivia
    {
        public override string ToString()
        {
            return Text;
        }
    }
}