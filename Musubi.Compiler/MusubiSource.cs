namespace Musubi.Compiler
{
    public readonly struct MusubiSource(string source, string filename)
    {
        public readonly string Content = source;
        public readonly string FullPath = filename;
    }
}
