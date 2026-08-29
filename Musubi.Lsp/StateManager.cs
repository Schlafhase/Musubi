using System.Collections.Concurrent;
using Musubi.Compiler;
using Musubi.Compiler.Nodes;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;
using Buffer = Microsoft.Language.Xml.Buffer;

namespace Musubi.Lsp
{
    public class StateManager
    {
        private readonly ConcurrentDictionary<string, Buffer> _buffers = new();

        private Node? _ast;
        private List<Token>? _tokens;
        private Errors? _errors;

        public void UpdateBuffer(string documentPath, Buffer buffer)
        {
            _buffers.AddOrUpdate(documentPath, buffer, (_, __) => buffer);
            Reparse(documentPath);
        }

        public Buffer? GetBuffer(string documentPath)
        {
            return _buffers.TryGetValue(documentPath, out var buffer) ? buffer : null;
        }

        public string? GetBufferText(string documentPath)
        {
            Buffer? buffer = GetBuffer(documentPath);
            return buffer?.GetText(0, buffer.Length);
        }

        public void Reparse(string documentPath)
        {
            Buffer buffer = _buffers[documentPath];

            _errors = new();
            _errors.FilenameToSource.Add(documentPath, buffer.GetText(0, buffer.Length));
            Scanner s = new(buffer.GetText(0, buffer.Length), documentPath, _errors);
            ScannerResult result = s.ScanTokensFullInfo();
            _tokens = result.FullTokens;
            Parser p = new(result.ParsableTokens, _errors);
            _ast = p.Parse();
            if (
                documentPath.EndsWith(".mbim")
                && _errors.Items.Count > 0
                && _errors.Items[^1].Message == "Expected expression but got EOF instead"
            )
            {
                _errors.Items.RemoveAt(_errors.Items.Count - 1);
            }
        }

        public List<ErrorsReportable> GetErrors()
        {
            return _errors?.Items ?? [];
        }

        public Node? GetAST()
        {
            return _ast;
        }

        public List<Token>? GetTokens()
        {
            return _tokens;
        }
    }
}
