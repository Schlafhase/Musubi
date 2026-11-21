namespace Musubi.Compiler.Scanning
{
    public sealed record Comment(string Content) : Trivia
    {
        public override string ToString()
        {
            return ";" + Content;
        }
    }
}