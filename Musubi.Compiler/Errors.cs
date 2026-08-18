using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public enum Severity
    {
        Hint,
        Warning,
        Error,
    }

    // TODO: refactor to use CodeLocation and CodeRange
    public abstract record ErrorsReportable(string Message, Severity Severity);

    public record ErrorsReportableWithLocation(
        string Message,
        string Filename,
        int Line,
        int Column,
        int Length,
        Severity Severity
    ) : ErrorsReportable(Message, Severity);

    public class Errors
    {
        public bool HasErrors { get; private set; }
        public readonly Dictionary<string, string> FilenameToSource = [];
        public List<ErrorsReportable> Items = [];

        public void Report(ErrorsReportable item)
        {
            writeColoured(item.Severity + ": " + item.Message, item.Severity);
        }

        public void ReportAll()
        {
            foreach (ErrorsReportable item in Items)
            {
                if (item is ErrorsReportableWithLocation l)
                {
                    ReportWithLocation(l);
                }
                else
                {
                    Report(item);
                }
            }
        }

        public void ReportWithLocation(ErrorsReportableWithLocation item)
        {
            if (!FilenameToSource.TryGetValue(item.Filename, out var source))
            {
                throw new KeyNotFoundException($"No source for {item.Filename} was provided");
            }
            int length = item.Length >= 1 ? item.Length : 1;
            string[] lines = source.Split('\n');
            int line = item.Line > lines.Length ? lines.Length : item.Line;
            string lineNo = $"{line} | ";
            string sourceLine = lines[line - 1].TrimEnd();

            int column = item.Column > sourceLine.Length ? sourceLine.Length : item.Column;
            string padding = new(' ', lineNo.Length + column - 1);
            string indicator = new('^', length);
            writeColoured(item.Filename + ":", item.Severity);
            writeColoured(line + ":" + column + ":", item.Severity);
            Report(item); // for the message
            Console.WriteLine(lineNo + sourceLine);
            writeColoured(padding + indicator + " Here", item.Severity);
            Console.WriteLine();
        }

        public void Error(string message, string filename, int line, int column, int length = 1)
        {
            HasErrors = true;
            Items.Add(
                new ErrorsReportableWithLocation(
                    message,
                    filename,
                    line,
                    column,
                    length,
                    Severity.Error
                )
            );
        }

        private void writeColoured(string message, Severity severity)
        {
            ConsoleColor c = severity switch
            {
                Severity.Hint => ConsoleColor.Blue,
                Severity.Warning => ConsoleColor.Yellow,
                Severity.Error => ConsoleColor.Red,
                _ => ConsoleColor.Gray,
            };
            ConsoleColor previous = Console.ForegroundColor;
            Console.ForegroundColor = c;
            Console.WriteLine(message);
            Console.ForegroundColor = previous;
        }

        private void token(string message, Token t, Action<string, string, int, int, int> addItem)
        {
            addItem(message, t.Filename, t.Line, t.Column, t.Lexeme.Length);
        }

        public void TokenError(Token t, string message)
        {
            token(message, t, Error);
        }

        public void TokenError(Token t)
        {
            token($"Unexpected token of type '{t.Type}'", t, Error);
        }

        public void UnexpectedError(Token actual, params TokenType[] expected)
        {
            token(
                $"Expected {(expected.Length > 1 ? "one of " : "")}{string.Join(", ", expected)} but got {actual.Type} instead",
                actual,
                Error
            );
        }

        public void UnexpectedError(Token actual, string expected)
        {
            token($"Expected {expected} but got {actual.Type} instead", actual, Error);
        }

        public void Warning(string message, string filename, int line, int column, int length = 1)
        {
            Items.Add(
                new ErrorsReportableWithLocation(
                    message,
                    filename,
                    line,
                    column,
                    length,
                    Severity.Warning
                )
            );
        }

        public void TokenWarning(Token t, string message)
        {
            token(message, t, Warning);
        }

        public void Hint(string message, string filename, int line, int column, int length = 1)
        {
            Items.Add(
                new ErrorsReportableWithLocation(
                    message,
                    filename,
                    line,
                    column,
                    length,
                    Severity.Hint
                )
            );
        }

        public void TokenHint(Token t, string message)
        {
            token(message, t, Hint);
        }
    }
}
