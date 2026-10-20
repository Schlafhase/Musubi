using Musubi.Compiler;
using Musubi.Compiler.Scanning;
using OmniSharp.Extensions.LanguageServer.Protocol.Client.Capabilities;
using OmniSharp.Extensions.LanguageServer.Protocol.Document;
using OmniSharp.Extensions.LanguageServer.Protocol.Models;
using static Musubi.Lsp.StateManager;

namespace Musubi.Lsp
{
    public class CompletionHandler(StateManager stateManager) : CompletionHandlerBase
    {
        private static readonly string[] _keywords =
        [
            "let",
            "in",
            "#define",
            "#once",
            "#include",
            "infix",
            "infixl",
            "infixr",
        ];

        public override Task<CompletionItem> Handle(
            CompletionItem request,
            CancellationToken cancellationToken
        )
        {
            return Task.FromResult(request);
        }

        public override Task<CompletionList> Handle(
            CompletionParams request,
            CancellationToken cancellationToken
        )
        {
            string path = request.TextDocument.Uri.ToString();
            CompilationData? data = stateManager.GetCompilationData(path);

            List<(Token t, CodeRange r)> symbols = [];
            if (data is { Contexts: not null } data_)
            {
                symbols =
                [
                    .. data_.Contexts
                    //
                    // .Where(c => positionInRange(request.Position, c.range))
                    .Select(c => (c.symbol, c.range)),
                ];
                symbols.Add(
                    (
                        new Token(TokenType.Identifier, "heheheheh", 0, 0, ""),
                        new CodeRange(new("", 0, 0), new("", 0, 0))
                    )
                );
            }

            List<CompletionItem> items =
            [
                .. symbols.Select(s => new CompletionItem()
                {
                    Label = s.t.Lexeme,
                    Kind = CompletionItemKind.Variable,
                    Documentation = s.r.ToString(),
                }),
            ];

            items.AddRange(
                _keywords.Select(kw => new CompletionItem()
                {
                    Label = kw,
                    Kind = CompletionItemKind.Keyword,
                })
            );

            return Task.FromResult(new CompletionList(items, false));
        }

        protected override CompletionRegistrationOptions CreateRegistrationOptions(
            CompletionCapability capability,
            ClientCapabilities clientCapabilities
        )
        {
            return new CompletionRegistrationOptions
            {
                DocumentSelector = TextDocumentSelector.ForLanguage("musubi", "musubi-module"),
            };
        }

        private static bool positionInRange(Position p, CodeRange r)
        {
            if (p.Line + 1 >= r.A.Line && p.Line + 1 <= r.B.Line)
            {
                if (p.Line + 1 != r.B.Line)
                {
                    return true;
                }
                return r.A.Column <= p.Character && p.Character + 1 <= r.B.Column;
            }
            return false;
        }
    }
}
