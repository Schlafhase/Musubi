namespace Musubi.Compiler.Scanning
{
    public enum TokenType
    {
        EOF,

        LeftParen,
        RightParen,
        Dot,
        Lambda,
        Definition,
        ListSeparator,

        Identifier,

        // literals
        Number,
        ScottNumber,
        ChurchNumber,
        BinaryNumber,
        String,
        Char,
        Filename,

        // Keywords
        Let,
        In,
        Infix,
        Infixl,
        Infixr,

        // tokens that will get expanded or converted to trivia in the expander
        IncludeOnce,
        Define,
        Include,

        // marker tokens (produced by the expander)
        BeginInclude,
        EndInclude,
        BeginMacro,
        EndMacro,
    }
}
