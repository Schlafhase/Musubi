using Microsoft.Extensions.DependencyInjection;
using Musubi.Lsp;
using OmniSharp.Extensions.LanguageServer.Server;

static void ConfigureServices(IServiceCollection services)
{
    services.AddSingleton<StateManager>();
}

LanguageServer server = await LanguageServer.From(options =>
    options
        .WithInput(Console.OpenStandardInput())
        .WithOutput(Console.OpenStandardOutput())
        .AddDefaultLoggingProvider()
        .WithServices(ConfigureServices)
        .WithHandler<TextDocumentSyncHandler>()
// .WithHandler<SemanticTokensHandler>() // highlighting is now handled by the treesitter parser
);

await server.WaitForExit;
