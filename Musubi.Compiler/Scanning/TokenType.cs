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

        Identifier,
        Filename,
        Number,

        Let,
        Include,
        In,

        EOF,
    }
}