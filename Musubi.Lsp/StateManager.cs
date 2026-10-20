using System.Collections.Concurrent;
using Microsoft.Extensions.Logging;
using Musubi.Compiler;
using Musubi.Compiler.Nodes;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;
using static Musubi.Compiler.Parsing.Parser;
using Buffer = Microsoft.Language.Xml.Buffer;

namespace Musubi.Lsp
{
    public class StateManager(ILogger<StateManager> logger)
    {
        private readonly ConcurrentDictionary<string, Buffer> _buffers = new();
        private readonly ConcurrentDictionary<string, CompilationData> _data = new();

        public struct CompilationData
        {
            public Node? AST;
            public IReadOnlyList<Token>? Tokens;
            public IReadOnlyList<Token>? HoverTokens;
            public IReadOnlyList<(CodeRange range, Token symbol)>? Contexts;
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
            data.Contexts = r.Contexts;
            logger.LogDebug(
                $"CONTEXTS ({r.Contexts.Count}):" +
                string.Join(", ", r.Contexts.Select(c => c.symbol.Lexeme))
            );
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

        public IReadOnlyList<Token>? GetHoverTokens(string documentPath)
        {
            return _data[documentPath].HoverTokens;
        }

        public IReadOnlyList<Token>? GetTokens(string documentPath)
        {
            return _data[documentPath].Tokens;
        }

        public CompilationData? GetCompilationData(string documentPath)
        {
            return _data.TryGetValue(documentPath, out CompilationData data) ? data : null;
        }
    }
}
