using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Musubi.Lsp;
using OmniSharp.Extensions.LanguageServer.Server;
using Serilog;

static void ConfigureServices(IServiceCollection services)
{
    services.AddSingleton<StateManager>();
}

Log.Logger = new LoggerConfiguration()
    .MinimumLevel.Verbose()
    .WriteTo.File("/home/Linus/Projects/cs/Musubi/lsp.log")
    .CreateLogger();

LanguageServer server = await LanguageServer.From(options =>
    options
        .WithInput(Console.OpenStandardInput())
        .WithOutput(Console.OpenStandardOutput())
        .AddDefaultLoggingProvider()
        .WithServices(ConfigureServices)
        .WithHandler<TextDocumentSyncHandler>()
        .WithHandler<HoverHandler>()
        .WithHandler<CompletionHandler>()
        .ConfigureLogging(l =>
            l.ClearProviders()
                .AddSerilog()
                .AddConsole(o => o.LogToStandardErrorThreshold = LogLevel.Trace)
                .AddLanguageProtocolLogging()
                .SetMinimumLevel(LogLevel.Trace)
        )
// .WithHandler<SemanticTokensHandler>() // highlighting is now handled by the treesitter parser
);

await server.WaitForExit;
