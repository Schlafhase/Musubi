using Musubi.Compiler.Scanning;
using OmniSharp.Extensions.LanguageServer.Protocol.Client.Capabilities;
using OmniSharp.Extensions.LanguageServer.Protocol.Document;
using OmniSharp.Extensions.LanguageServer.Protocol.Models;

namespace Musubi.Lsp
{
    public class SemanticTokensHandler : SemanticTokensHandlerBase
    {
        private readonly StateManager _stateManager;

        public SemanticTokensHandler(StateManager stateManager) => _stateManager = stateManager;

        protected override SemanticTokensRegistrationOptions CreateRegistrationOptions(
            SemanticTokensCapability capability,
            ClientCapabilities clientCapabilities
        )
        {
            return new SemanticTokensRegistrationOptions
            {
                DocumentSelector = TextDocumentSelector.ForLanguage("musubi"),
                Legend = new SemanticTokensLegend
                {
                    TokenTypes = new Container<SemanticTokenType>(SemanticTokenType.Defaults),
                    TokenModifiers = new Container<SemanticTokenModifier>(
                        SemanticTokenModifier.Defaults
                    ),
                },
                Full = new SemanticTokensCapabilityRequestFull { Delta = true },
                Range = true,
            };
        }

        protected override Task<SemanticTokensDocument> GetSemanticTokensDocument(
            ITextDocumentIdentifierParams @params,
            CancellationToken cancellationToken
        )
        {
            return Task.FromResult(new SemanticTokensDocument(RegistrationOptions.Legend));
        }

        protected override async Task Tokenize(
            SemanticTokensBuilder builder,
            ITextDocumentIdentifierParams identifier,
            CancellationToken cancellationToken
        )
        {
            string? text = _stateManager.GetBufferText(identifier.TextDocument.Uri.ToString());
            List<Token>? tokens = _stateManager.GetTokens(identifier.TextDocument.Uri.ToString());
            if (text is null || tokens is null)
            {
                await Task.CompletedTask;
                return;
            }

            foreach (
                Token t in tokens.Where(t => t.Filename == identifier.TextDocument.Uri.ToString())
            )
            {
                SemanticTokenType? type = t.Type switch
                {
                    TokenType.BinaryNumber => SemanticTokenType.Number,
                    TokenType.ChurchNumber => SemanticTokenType.Number,
                    TokenType.Definition => SemanticTokenType.Operator,
                    TokenType.Dot => SemanticTokenType.Keyword,
                    TokenType.Lambda => SemanticTokenType.Keyword,
                    TokenType.ListSeparator => SemanticTokenType.Keyword,
                    TokenType.Identifier => SemanticTokenType.Variable,
                    TokenType.Filename => SemanticTokenType.String,
                    TokenType.Number => SemanticTokenType.Number,
                    TokenType.ScottNumber => SemanticTokenType.Number,
                    TokenType.Let => SemanticTokenType.Keyword,
                    TokenType.In => SemanticTokenType.Keyword,
                    TokenType.Infix => SemanticTokenType.Keyword,
                    TokenType.Infixl => SemanticTokenType.Keyword,
                    TokenType.Infixr => SemanticTokenType.Keyword,
                    _ => null!,
                };

                if (type is null)
                {
                    continue;
                }

                builder.Push(t.Line - 1, t.Column - 1, t.Lexeme.Length, type);
            }
            builder.Commit();

            await Task.CompletedTask;
        }
    }
}