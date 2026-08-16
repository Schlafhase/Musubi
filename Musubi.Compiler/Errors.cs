using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public class Errors(string source, string? filename = null)
    {
        public bool HasErrors { get; private set; }

        public void Report(string message, int line, int column, int length = 1)
        {
            length = length >= 1 ? length : 1;
            HasErrors = true;
            string lineNo = $"{line} | ";
            string sourceLine = source.Split('\n')[line - 1].TrimEnd();
            string padding = new(' ', lineNo.Length + column - 1);
            string indicator = new('^', length);
            Console.ForegroundColor = ConsoleColor.Red;
            if (filename is not null)
            {
                Console.Write(filename + ":");
            }
            Console.WriteLine(line + ":" + column + ":");
            Console.WriteLine(message);
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine(lineNo + sourceLine);
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(padding + indicator + " Here");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine();
        }

        public void ReportToken(Token t, string message)
        {
            Report(message, t.Line, t.Column, t.Lexeme.Length);
        }

        public void ReportToken(Token t)
        {
            Report($"Unexpected token of type '{t.Type}'", t.Line, t.Column, t.Lexeme.Length);
        }

        public void ReportUnexpected(Token actual, params TokenType[] expected)
        {
            Report(
                $"Expected {(expected.Length > 1 ? "one of " : "")}{string.Join(", ", expected)} but got {actual.Type} instead",
                actual.Line,
                actual.Column,
                actual.Lexeme.Length
            );
        }

        public void ReportUnexpected(Token actual, string expected)
        {
            Report(
                $"Expected {expected} but got {actual.Type} instead",
                actual.Line,
                actual.Column,
                actual.Lexeme.Length
            );
        }
    }
}
