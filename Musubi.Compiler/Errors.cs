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
        public static readonly ErrorKindNoArgs CircularInclude = new(
            5,
            Severity.Error,
            "This include would recurse infinitely."
        );
        public static readonly ErrorKindNoArgs FileNotFound = new(
            6,
            Severity.Error,
            "This file couldn't be found in any library directory or relative to the source file."
        );
        public static readonly ErrorKindNoArgs TokenNotAllowedInMacro = new(
            7,
            Severity.Error,
            "Macros mustn't contain #define or #include since the compiler pipeline doesn't support handling these."
        );
        public static readonly ErrorKindNoArgs NewlineInDirective = new(
            8,
            Severity.Error,
            "Newlines between directives and their parameters are not allowed."
        );
    }
}
