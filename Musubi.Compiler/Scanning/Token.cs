namespace Musubi.Compiler.Scanning
{
    public class Token(TokenType type, string lexeme, int line, int column, string filename)
    {
        public readonly TokenType Type = type;
        public readonly string Lexeme = lexeme;
        public object? Literal { get; init; }
        public readonly int Line = line;
        public readonly int Column = column;
        public readonly string Filename = filename;

        public override string ToString()
        {
            return Type + " " + Lexeme + " at line " + Line + " column " + Column;
        }
    }
}