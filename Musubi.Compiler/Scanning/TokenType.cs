namespace Musubi.Compiler.Scanning
{
    public enum TokenType
    {
        LeftParen,
        RightParen,
        Dot,
        Lambda,
        Definition,
        ListSeparator,

        // literals
        Identifier,
        Filename,
        Number,
        ChurchNumber,

        Let,
        In,

        IncludeOnce,

        EOF,
    }
}