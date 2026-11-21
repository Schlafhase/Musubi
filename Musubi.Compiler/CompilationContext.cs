namespace Musubi.Compiler
{
    public readonly struct CompilationContext(MusubiSource source, ErrorLogger errorLogger)
    {
        public readonly MusubiSource Source = source;
        public readonly ErrorLogger ErrorLogger = errorLogger;
    }
}
