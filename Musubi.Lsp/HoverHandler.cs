using System.Collections.Immutable;
using Musubi.Compiler;
using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;
using OmniSharp.Extensions.LanguageServer.Protocol.Client.Capabilities;
using OmniSharp.Extensions.LanguageServer.Protocol.Document;
using OmniSharp.Extensions.LanguageServer.Protocol.Models;

namespace Musubi.Lsp
{
    public class HoverHandler(StateManager stateManager) : HoverHandlerBase
    {
        public override async Task<Hover?> Handle(
            HoverParams request,
            CancellationToken cancellationToken
        )
        {
            List<Token>? hoverTokens = stateManager.GetHoverTokens(
                request.TextDocument.Uri.ToString()
            );
            Token? t = hoverTokens is not null
                ? getAt(hoverTokens, request.Position, request.TextDocument.Uri.ToString())
                : null;
            return t is null || t.DocString is null
                ? null
                : new Hover()
                {
                    Contents = new MarkedStringsOrMarkupContent(
                        new MarkupContent() { Kind = MarkupKind.Markdown, Value = t.DocString }
                    ),
                };
        }

        protected override HoverRegistrationOptions CreateRegistrationOptions(
            HoverCapability capability,
            ClientCapabilities clientCapabilities
        )
        {
            return new HoverRegistrationOptions()
            {
                // DocumentSelector = TextDocumentSelector.ForLanguage("musubi"),
            };
        }

        private static Lambda? getLambdaForVariable(
            Node ast,
            Variable variable,
            ImmutableStack<Lambda> lambdas
        )
        {
            if (ast.Id == variable.Id)
            {
                Lambda? l = null;
                for (int i = 0; i < variable.DeBruijn; i++)
                {
                    lambdas = lambdas.Pop(out l);
                }
                return l;
            }

            Node n = ast;
            return n switch
            {
                Document d => getLambdaForVariable(d.Expression, variable, lambdas),
                Lambda l => getLambdaForVariable(l.Body, variable, lambdas.Push(l)),
                Application a => getLambdaForVariable(a.Function, variable, lambdas)
                    ?? getLambdaForVariable(a.Argument, variable, lambdas),
                _ => null,
            };
        }

        private static Token? getAt(List<Token> hoverTokens, Position p, string filename)
        {
            foreach (Token t in hoverTokens)
            {
                if (t.Filename == filename && positionInRange(p, t.Range))
                {
                    return t;
                }
            }
            return null;
        }

        private static bool positionInRange(Position p, CodeRange r)
        {
            if (p.Line + 1 >= r.A.Line && p.Line + 1 <= r.B.Line)
            {
                if (p.Line + 1 != r.B.Line)
                {
                    return true;
                }
                return p.Character + 1 <= r.B.Column;
            }
            return false;
        }
    }
}
