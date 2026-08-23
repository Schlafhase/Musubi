using CommandDotNet;
using CommandDotNet.Help;

using Musubi.Cli;

return new AppRunner<App>(new AppSettings(){Help = new AppHelpSettings() {UsageAppName = "musubi"}}).Run(args);