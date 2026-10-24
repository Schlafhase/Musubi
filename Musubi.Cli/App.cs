using System.Diagnostics;
using CommandDotNet;
using Musubi.Compiler;

namespace Musubi.Cli
{
    public class App
    {
        [Command("compile", Description = "Compiles a Musubi program to another language")]
        public void Compile(
            [Operand("file", Description = "Path to the Musubi program")]
                string filepath = "main.mbi",
            [Option('l', Description = "Language to compile to")]
                TargetLanguage targetLanguage = TargetLanguage.C,
            [Option('o', Description = "Name of the resulting file")]
                string outputFile = "compiled",
            [Option(
                'i',
                Description = "Directories which will be used to find #included files. ~/.musubi is always searched",
                Split = ' '
            )]
                List<string>? libraryDirs = null,
            [Option('d', Description = "Print additional debug information (like tokens, AST)")]
                bool printAST = false
        )
        {
            libraryDirs = qualifyLibraryDirs(libraryDirs);
            string source;
            try
            {
                source = File.ReadAllText(filepath);
            }
            catch (Exception e)
            {
                Console.WriteLine($"Failed to read '{filepath}': {e.Message}");
                return;
            }
            string result;
            try
            {
                result = MusubiCompiler.Compile(
                    source,
                    filepath,
                    printAST,
                    targetLanguage,
                    libraryDirs
                );
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return;
            }
            string resultFile =
                outputFile == "compiled"
                    ? (
                        targetLanguage switch
                        {
                            TargetLanguage.C => "compiled.c",
                            TargetLanguage.LC => "compiled.lam",
                            _ => "compiled",
                        }
                    )
                    : outputFile;
            File.WriteAllText(resultFile, result);
            Console.WriteLine($"Compiled and wrote to {resultFile}");
        }

        [Command("run", Description = "Runs a Musubi program")]
        public void Run(
            [Operand("file", Description = "Path to the Musubi program")]
                string filepath = "main.mbi",
            [Option(
                'i',
                Description = "Directories which will be used to find #included files. ~/.musubi is always searched",
                Split = ' '
            )]
                List<string>? libraryDirs = null,
            [Option('c', Description = "The C compiler to use")] string cc = "gcc",
            [Option('d', Description = "Print additional debug information (like tokens, AST)")]
                bool printAST = false
        )
        {
            libraryDirs = qualifyLibraryDirs(libraryDirs);
            string source;
            try
            {
                source = File.ReadAllText(filepath);
            }
            catch (Exception e)
            {
                Console.WriteLine($"Failed to read '{filepath}': {e.Message}");
                return;
            }
            string result;
            try
            {
                result = MusubiCompiler.Compile(
                    source,
                    filepath,
                    printAST,
                    TargetLanguage.C,
                    libraryDirs
                );
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
                return;
            }
            string resultFile = "/tmp/" + Guid.NewGuid().ToString() + ".c";
            File.WriteAllText(resultFile, result);

            // NOTE: this assumes that the musubi compiler always outputs valid C
            // (which should be the case but since this is a hobby project, might not always be true)
            ProcessStartInfo ccPsi = new(cc, $"{resultFile} -o {resultFile}.out")
            {
                CreateNoWindow = true,
            };

            try
            {
                using Process ccProcess = new() { StartInfo = ccPsi };
                ccProcess.Start();
                ccProcess.WaitForExit();

                ProcessStartInfo programPsi = new(resultFile + ".out") { CreateNoWindow = true };
                using Process p = new() { StartInfo = programPsi };
                p.Start();
                p.WaitForExit();

                if (p.ExitCode != 0)
                {
                    Console.WriteLine(
                        "The process crashed. This is most likely because the output of the program wasn't in the correct format (scott list of integers representing a string). This can also be caused by a stack overflow."
                    );
                }
            }
            finally
            {
                File.Delete(resultFile);
                File.Delete(resultFile + ".out");
            }
        }

        private static List<string> qualifyLibraryDirs(List<string>? dirs)
        {
            if (dirs is null)
            {
                return [];
            }
            List<string> absoluteDirs = [];
            foreach (string dir in dirs)
            {
                if (!Path.IsPathFullyQualified(dir))
                {
                    absoluteDirs.Add(Path.GetFullPath(dir));
                }
                else
                {
                    absoluteDirs.Add(dir);
                }
            }
            List<string> badDirs = [.. absoluteDirs.Where(d => !Directory.Exists(d))];
            foreach (string dir in badDirs)
            {
                Console.WriteLine($"Hint: Specified library directory does not exist: {dir}");
            }

            if (badDirs.Count > 0)
            {
                Console.WriteLine();
            }
            return absoluteDirs;
        }
    }
}
