using System.Collections.Immutable;
using Musubi.Compiler;
using Musubi.Compiler.Nodes;
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
            Node? ast = stateManager.GetAST(request.TextDocument.Uri.ToString());
            Node? n = ast is not null ? getAt(ast, request.Position) : null;
            return new Hover()
            {
                Contents = new MarkedStringsOrMarkupContent(
                    new MarkupContent()
                    {
                        Kind = MarkupKind.Markdown,
                        Value = n is Variable v
                            ? getLambdaForVariable(n, v, [])?.VariableDocString ?? "no docstring"
                            : n?.GetType().Name
                                + " "
                                + n?.Range
                                + " "
                                + $"{request.Position.Line}:{request.Position.Character}"
                                ?? "null",
                    }
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

        private static Node? getAt(Node n, Position p)
        {
            if (!positionInRange(p, n.Range))
            {
                return null;
            }

            return n switch
            {
                Document d => getAt(d.Expression, p) ?? n,
                Lambda l => getAt(l.Body, p) ?? n,
                Application a => getAt(a.Function, p) ?? getAt(a.Argument, p) ?? n,
                _ => n,
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
                return p.Character + 1 <= r.B.Column;
            }
            return false;
        }
    }
}
