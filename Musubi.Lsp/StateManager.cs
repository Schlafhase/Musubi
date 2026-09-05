using System.Collections.Concurrent;
using Musubi.Compiler;
using Musubi.Compiler.Nodes;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;
using static Musubi.Compiler.Parsing.Parser;
using Buffer = Microsoft.Language.Xml.Buffer;

namespace Musubi.Lsp
{
    public class StateManager
    {
        private readonly ConcurrentDictionary<string, Buffer> _buffers = new();
        private readonly ConcurrentDictionary<string, CompilationData> _data = new();

        private struct CompilationData
        {
            public Node? AST;
            public List<Token>? Tokens;
            public List<Token>? HoverTokens;
            public Errors Errors = new();

            public CompilationData() { }
        }

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

            CompilationData data = new();
            data.Errors.FilenameToSource.Add(documentPath, buffer.GetText(0, buffer.Length));
            Scanner s = new(buffer.GetText(0, buffer.Length), documentPath, data.Errors);
            ScannerResult result = s.ScanTokensFullInfo();
            data.Tokens = result.FullTokens;
            Parser p = new(result.ParsableTokens, data.Errors);
            ParseResult r = p.Parse();
            data.AST = r.Root;
            data.HoverTokens = r.HoverTokens;
            if (
                documentPath.EndsWith(".mbim")
                && data.Errors.Items.Count > 0
                && data.Errors.Items[^1].Message == "Expected expression but got EOF instead"
            )
            {
                data.Errors.Items.RemoveAt(data.Errors.Items.Count - 1);
            }
            _data[documentPath] = data;
        }

        public List<ErrorsReportable> GetErrors(string documentPath)
        {
            return _data[documentPath].Errors?.Items ?? [];
        }

        public Node? GetAST(string documentPath)
        {
            return _data[documentPath].AST;
        }

        public List<Token>? GetHoverTokens(string documentPath)
        {
            return _data[documentPath].HoverTokens;
        }

        public List<Token>? GetTokens(string documentPath)
        {
            return _data[documentPath].Tokens;
        }
    }
}
