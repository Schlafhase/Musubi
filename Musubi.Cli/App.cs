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
            [Option('d', Description = "Print additional debug information (like tokens, AST)")]
                bool printAST = false,
            [Option(
                'i',
                Description = "Directories which will be used to find #included files. ~/.musubi is always searched",
                Split = ' '
            )]
                List<string>? libraryDirs = null
        )
        {
            if (!validateLibraryDirs(libraryDirs))
            {
                return;
            }
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
            [Option('d', Description = "Print additional debug information (like tokens, AST)")]
                bool printAST = false,
            [Option(
                'i',
                Description = "Directories which will be used to find #included files. ~/.musubi is always searched",
                Split = ' '
            )]
                List<string>? libraryDirs = null
        )
        {
            if (!validateLibraryDirs(libraryDirs))
            {
                return;
            }
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
            string resultFile = Guid.NewGuid().ToString() + ".c";
            File.WriteAllText(resultFile, result);

            // NOTE: this assumes that the musubi compiler always outputs valid C
            // (which should be expected but since this is a hobby project, might not always be true)
            ProcessStartInfo gccPsi = new("gcc", $"{resultFile} -o {resultFile}.out")
            {
                CreateNoWindow = true,
            };
            using Process gcc = new() { StartInfo = gccPsi };
            gcc.Start();
            gcc.WaitForExit();

            ProcessStartInfo programPsu = new(resultFile + ".out") { CreateNoWindow = true };
            using Process p = new() { StartInfo = programPsu };
            p.Start();
            p.WaitForExit();

            if (p.ExitCode != 0)
            {
                Console.WriteLine(
                    "The process crashed. This is most likely because the output of the program wasn't in the correct format (scott list of integers representing a string). This can also be caused by a stack overflow."
                );
            }
            File.Delete(resultFile);
            File.Delete(resultFile + ".out");
        }

        private bool validateLibraryDirs(List<string>? dirs)
        {
            if (dirs is null)
            {
                return true;
            }
            List<string> badDirs = [];
            foreach (string dir in dirs)
            {
                if (!Path.IsPathFullyQualified(dir))
                {
                    badDirs.Add(dir);
                }
            }
            if (badDirs.Count == 0)
            {
                return true;
            }
            Console.WriteLine(
                "Library directories have to be fully qualified paths (absolute paths). The following library directories are not fully qualified:"
            );
            foreach (string dir in badDirs)
            {
                Console.WriteLine(" " + dir);
            }
            return false;
        }
    }
}
