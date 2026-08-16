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
        Number,

        Let,
        In,
        Then,
        Else,

        EOF,
    }
}
