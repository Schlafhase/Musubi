namespace Musubi.Compiler.Scanning
{
    public record Token(TokenType Type, string Lexeme, int Line, int Column, string Filename)
    {
        public object? Literal { get; init; }
        public string? DocString { get; init; }
        public CodeRange Range =>
            new(new(Filename, Line, Column), new(Filename, Line, Column + Lexeme.Length));
        public CodeRange? IncludedAt;

        public override string ToString()
        {
            return Type + " " + Lexeme + " at line " + Line + " column " + Column;
        }

        // public Token(Token t)
        // {
        //     Type = t.Type;
        //     Lexeme = t.Lexeme;
        //     Line = t.Line;
        //     Column = t.Column;
        //     Filename = t.Filename;
        // }
    }
}
