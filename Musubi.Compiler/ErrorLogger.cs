
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

        public void AddDiagnostic(CodeRange range, ErrorKindNoArgs kind)
        {
            add(kind, kind.Message(), range);
        }

        public void AddDiagnostic<T0>(CodeRange range, ErrorKind<T0> kind, T0 a)
        {
            add(kind, kind.Message(a), range);
        }

        public void AddDiagnostic<T0, T1>(CodeRange range, ErrorKind<T0, T1> kind, T0 a, T1 b)
        {
            add(kind, kind.Message(a, b), range);
        }

        public void AddDiagnostic<T0, T1, T2>(
            CodeRange range,
            ErrorKind<T0, T1, T2> kind,
            T0 a,
            T1 b,
            T2 c
        )
        {
            add(kind, kind.Message(a, b, c), range);
        }

        private void add(ErrorKind kind, string message, CodeRange range)
        {
            if (range.Length is int length)
            {
                HasErrors = HasErrors || kind.Severity == Severity.Error;
                _items.Add(
                    new DiagnosticWithLocation(
                        message,
                        kind,
                        range.A.Filename,
                        range.A.Line,
                        range.A.Column,
                        length
                    )
                );
            }
            else
            {
                throw new NotImplementedException(
                    "Diagnostics spanning over multiple lines are not supported (yet?)"
                );
            }
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
