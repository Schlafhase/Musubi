namespace Musubi.Compiler.Scanning
{
    public enum TokenType
    {
        LeftParen,
        RightParen,
        Dot,
        Lambda,
        Definition,
        StatementEnd,

        // literals
        Identifier,
        Filename,
        Number,

        Let,
        In,

        IncludeOnce,

        EOF,
    }
}