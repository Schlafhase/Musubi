namespace Musubi.Compiler.Exceptions
{
    public class DuplicateIncludeWarning : Exception
    {
        public DuplicateIncludeWarning() { }

        public DuplicateIncludeWarning(string? message)
            : base(message) { }

        public DuplicateIncludeWarning(string? message, Exception? innerException)
            : base(message, innerException) { }
    }
}
