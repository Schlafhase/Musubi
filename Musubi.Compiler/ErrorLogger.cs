using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public enum Severity
    {
        Hint,
        Warning,
        Error,
    }

    public abstract record Diagnostic(string Message, ErrorKind Kind);

    // TODO: refactor to use CodeLocation and CodeRange
    public record DiagnosticWithLocation(
        string Message,
        ErrorKind Kind,
        string Filename,
        int Line,
        int Column,
        int Length
    ) : Diagnostic(Message, Kind);

    public class ErrorLogger
    {
        public bool HasErrors { get; private set; }
        public readonly Dictionary<string, string> FilenameToSource = [];
        public IReadOnlyList<Diagnostic> Items => _items;

        private readonly List<Diagnostic> _items = [];

        public static void Report(Diagnostic item)
        {
            writeColoured(messageWithErrorCode(item.Kind, item.Message), item.Kind.Severity);
        }

        public void ReportAll()
        {
            foreach (Diagnostic item in _items)
            {
                if (item is DiagnosticWithLocation l)
                {
                    ReportWithLocation(l);
                }
                else
                {
                    Report(item);
                }
            }
        }

        public void ReportWithLocation(DiagnosticWithLocation item)
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
            writeColoured(item.Filename + ":", item.Kind.Severity);
            writeColoured(line + ":" + column + ":", item.Kind.Severity);
            Report(item); // for the message
            Console.WriteLine(lineNo + sourceLine);
            writeColoured(padding + indicator + " Here", item.Kind.Severity);
            Console.WriteLine();
        }

        public void AddDiagnostic(
            ErrorKindNoArgs kind,
            string filename,
            int line,
            int column,
            int length = 1
        )
        {
            add(kind, kind.Message(), filename, line, column, length);
        }

        public void AddDiagnostic<T0>(
            ErrorKind<T0> kind,
            T0 a,
            string filename,
            int line,
            int column,
            int length = 1
        )
        {
            add(kind, kind.Message(a), filename, line, column, length);
        }

        public void AddDiagnostic<T0, T1>(
            ErrorKind<T0, T1> kind,
            T0 a,
            T1 b,
            string filename,
            int line,
            int column,
            int length = 1
        )
        {
            add(kind, kind.Message(a, b), filename, line, column, length);
        }

        public void AddDiagnostic<T0, T1, T2>(
            ErrorKind<T0, T1, T2> kind,
            T0 a,
            T1 b,
            T2 c,
            string filename,
            int line,
            int column,
            int length = 1
        )
        {
            add(kind, kind.Message(a, b, c), filename, line, column, length);
        }

        private void add(
            ErrorKind kind,
            string message,
            string filename,
            int line,
            int column,
            int length = 1
        )
        {
            HasErrors = HasErrors || kind.Severity == Severity.Error;
            _items.Add(
                new DiagnosticWithLocation(
                         message,
                    kind,
                    filename,
                    line,
                    column,
                    length
                )
            );
        }

        private static string messageWithErrorCode(ErrorKind kind, string message)
        {
            return kind.Severity.ToString()
                + " MBI"
                + kind.Id.ToString().PadLeft(4, '0')
                + ": "
                + message;
        }

        private static void writeColoured(string message, Severity severity)
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
    }
}
