using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public class Errors
    {
        public bool HasErrors { get; private set; }
        public readonly Dictionary<string, string> FilenameToSource = [];

        public void Report(string message, string filename, int line, int column, int length = 1)
        {
            length = length >= 1 ? length : 1;
            HasErrors = true;
            string lineNo = $"{line} | ";
            if (!FilenameToSource.TryGetValue(filename, out var source))
            {
                throw new KeyNotFoundException($"No source for {filename} was provided");
            }
            string[] lines = source.Split('\n');
            line = line > lines.Length ? lines.Length : line;
            string sourceLine = lines[line - 1].TrimEnd();
            column = column > sourceLine.Length ? sourceLine.Length : column;
            string padding = new(' ', lineNo.Length + column - 1);
            string indicator = new('^', length);
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write(filename + ":");
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
            Report(message, t.Filename, t.Line, t.Column, t.Lexeme.Length);
        }

        public void ReportToken(Token t)
        {
            Report($"Unexpected token of type '{t.Type}'", t.Filename, t.Line, t.Column, t.Lexeme.Length);
        }

        public void ReportUnexpected(Token actual, params TokenType[] expected)
        {
            Report(
                $"Expected {(expected.Length > 1 ? "one of " : "")}{string.Join(", ", expected)} but got {actual.Type} instead",
                actual.Filename,
                actual.Line,
                actual.Column,
                actual.Lexeme.Length
            );
        }

        public void ReportUnexpected(Token actual, string expected)
        {
            Report(
                $"Expected {expected} but got {actual.Type} instead",
                actual.Filename,
                actual.Line,
                actual.Column,
                actual.Lexeme.Length
            );
        }
    }
}