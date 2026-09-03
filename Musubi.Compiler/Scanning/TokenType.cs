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
        ScottNumber,
        ChurchNumber,
        BinaryNumber,

        // Keywords
        Let,
        In,
        Infix,
        Infixl,
        Infixr,

        IncludeOnce, // not really a token (never reaches the parser)

        EOF,
    }
}
