namespace Musubi.Compiler
{
    public readonly struct MusubiSource(string source, string filename)
    {
        public readonly string Source = source;
        public readonly string Filename = filename;
    }
}
