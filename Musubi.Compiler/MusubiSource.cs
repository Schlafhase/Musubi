namespace Musubi.Compiler
{
    public readonly struct MusubiSource(string source, FileInfo fileinfo)
    {
        public readonly string Content = source;
        public readonly FileInfo File = fileinfo;
        public readonly string Directory = fileinfo.DirectoryName ?? throw new ArgumentException("File must be in a parent directory", nameof(fileinfo));
    }
}
