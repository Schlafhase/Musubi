namespace Musubi.Compiler.Scanning
{
    public sealed record BeginInclude(string IncludedFile) : Trivia;
}
