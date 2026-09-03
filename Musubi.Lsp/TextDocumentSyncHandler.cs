using MediatR;
using Microsoft.Language.Xml;
using Musubi.Compiler;
using OmniSharp.Extensions.LanguageServer.Protocol;
using OmniSharp.Extensions.LanguageServer.Protocol.Client.Capabilities;
using OmniSharp.Extensions.LanguageServer.Protocol.Document;
using OmniSharp.Extensions.LanguageServer.Protocol.Models;
using OmniSharp.Extensions.LanguageServer.Protocol.Server;
using OmniSharp.Extensions.LanguageServer.Protocol.Server.Capabilities;
using DiagnosticSeverity = OmniSharp.Extensions.LanguageServer.Protocol.Models.DiagnosticSeverity;

namespace Musubi.Lsp
{
    public class TextDocumentSyncHandler : TextDocumentSyncHandlerBase
    {
        private readonly ILanguageServerFacade _router;
        private readonly StateManager _stateManager;
        private readonly List<DocumentUri> _previousDiagnosticDocuments = [];

        private readonly TextDocumentSelector _documentSelector = TextDocumentSelector.ForLanguage(
            "musubi",
            "musubi-module"
        );

        public TextDocumentSyncHandler(ILanguageServerFacade router, StateManager bufferManager)
        {
            _router = router;
            _stateManager = bufferManager;
        }

        public TextDocumentSyncKind Change { get; } = TextDocumentSyncKind.Full;

        public override TextDocumentAttributes GetTextDocumentAttributes(DocumentUri uri) =>
            new(uri, "xml");

        public override Task<Unit> Handle(
            DidChangeTextDocumentParams request,
            CancellationToken cancellationToken
        )
        {
            string? text = request.ContentChanges.FirstOrDefault()?.Text;

            if (text is not null)
            {
                validate(request.TextDocument.Uri, text);
            }

            return Unit.Task;
        }

        protected override TextDocumentSyncRegistrationOptions CreateRegistrationOptions(
            TextSynchronizationCapability capability,
            ClientCapabilities clientCapabilities
        ) =>
            new()
            {
                DocumentSelector = _documentSelector,
                Change = Change,
                Save = new SaveOptions { IncludeText = true },
            };

        public override Task<Unit> Handle(
            DidOpenTextDocumentParams request,
            CancellationToken cancellationToken
        )
        {
            string text = request.TextDocument.Text;
            validate(request.TextDocument.Uri, text);

            return Unit.Task;
        }

        public override Task<Unit> Handle(
            DidSaveTextDocumentParams request,
            CancellationToken cancellationToken
        ) => Unit.Task;

        public override Task<Unit> Handle(
            DidCloseTextDocumentParams request,
            CancellationToken cancellationToken
        ) => Unit.Task;

        private void validate(DocumentUri uri, string text)
        {
            _stateManager.UpdateBuffer(uri.ToString(), new StringBuffer(text));
            var diagnostics = new Dictionary<DocumentUri, List<Diagnostic>>();

            foreach (ErrorsReportable error in _stateManager.GetErrors(uri.ToString()))
            {
                Diagnostic d;
                DocumentUri dUri = uri;
                if (error is ErrorsReportableWithLocation el)
                {
                    if (el.Filename != uri.ToString())
                    {
                        dUri = DocumentUri.FromFileSystemPath(el.Filename);
                    }
                    d = new()
                    {
                        Message = error.Message,
                        Range = new(
                            new Position(el.Line - 1, el.Column - 1),
                            new Position(el.Line - 1, el.Column + el.Length - 1)
                        ),
                        Severity = error.Severity switch
                        {
                            Severity.Hint => DiagnosticSeverity.Hint,
                            Severity.Warning => DiagnosticSeverity.Warning,
                            Severity.Error => DiagnosticSeverity.Error,
                            _ => throw new NotImplementedException(),
                        },
                        Source = "musubi",
                    };
                }
                else
                {
                    d = new()
                    {
                        Message = error.Message,
                        Severity = error.Severity switch
                        {
                            Severity.Hint => DiagnosticSeverity.Hint,
                            Severity.Warning => DiagnosticSeverity.Warning,
                            Severity.Error => DiagnosticSeverity.Error,
                            _ => throw new NotImplementedException(),
                        },
                        Source = "musubi",
                    };
                }
                if (diagnostics.TryGetValue(dUri, out List<Diagnostic>? entry))
                {
                    entry.Add(d);
                }
                else
                {
                    diagnostics[dUri] = [d];
                }
            }

            foreach (
                DocumentUri u in _previousDiagnosticDocuments.Where(u =>
                    !diagnostics.ContainsKey(u)
                )
            )
            {
                // clear diagnostics
                _router.TextDocument.PublishDiagnostics(
                    new PublishDiagnosticsParams { Uri = u, Diagnostics = [] }
                );
            }
            _previousDiagnosticDocuments.Clear();
            foreach (KeyValuePair<DocumentUri, List<Diagnostic>> kvp in diagnostics)
            {
                _previousDiagnosticDocuments.Add(kvp.Key);
                _router.TextDocument.PublishDiagnostics(
                    new PublishDiagnosticsParams
                    {
                        Uri = kvp.Key,
                        Diagnostics = new Container<Diagnostic>(kvp.Value),
                    }
                );
            }
        }
    }
}