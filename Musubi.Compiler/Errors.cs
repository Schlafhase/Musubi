using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public abstract record ErrorKind(int Id, Severity Severity, string Format);

    public sealed record ErrorKindNoArgs(int Id, Severity Severity, string Format)
        : ErrorKind(Id, Severity, Format)
    {
        public string Message() => Format;
    }

    public sealed record ErrorKind<T0>(int Id, Severity Severity, string Format)
        : ErrorKind(Id, Severity, Format)
    {
        public string Message(T0 a) => string.Format(Format, a);
    }

    public sealed record ErrorKind<T0, T1>(int Id, Severity Severity, string Format)
        : ErrorKind(Id, Severity, Format)
    {
        public string Message(T0 a, T1 b) => string.Format(Format, a, b);
    }

    public sealed record ErrorKind<T0, T1, T2>(int Id, Severity Severity, string Format)
        : ErrorKind(Id, Severity, Format)
    {
        public string Message(T0 a, T1 b, T2 c) => string.Format(Format, a, b, c);
    }

    public static class Errors
    {
        public static readonly ErrorKind<char> UnexpectedCharacter = new(
            0,
            Severity.Error,
            "Unexpected character '{0}'"
        );

        public static readonly ErrorKindNoArgs UnknownEscapeSequence = new(
            1,
            Severity.Error,
            "Unknown escape sequence. To escape a '\\' use '\\\\'."
        );

        public static readonly ErrorKind<string, string> UnexpectedText = new(
            2,
            Severity.Error,
            "Expected '{0}' but got '{1}'"
        );
        public static readonly ErrorKindNoArgs ExpectedFilename = new(
            3,
            Severity.Error,
            "Expected filename"
        );

        public static readonly ErrorKind<TokenType, TokenType> UnexpectedToken = new(
                4,
                Severity.Error,
                "Expected token of type '{0}' but got '{1}'"
                );
    }
}