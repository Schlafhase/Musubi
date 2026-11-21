namespace Musubi.Compiler.Exceptions
{
    public class CircularIncludeException : Exception {
        public CircularIncludeException()
        {
        }

        public CircularIncludeException(string? message) : base(message)
        {
        }

        public CircularIncludeException(string? message, Exception? innerException) : base(message, innerException)
        {
        }
    }
}
