using Musubi.Compiler.Nodes;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler
{
    public static class MusubiCompiler
    {
        public static string Compile(string source, string filename, bool printAST = false)
        {
            Errors e = new();
            e.FilenameToSource[filename] = source;

            Scanner s = new(source, filename, e);
            List<Token> tokens = s.ScanTokens();

            if (printAST)
            {
                foreach (Token t in tokens)
                {
                    Console.WriteLine(
                        t.Type
                            + " '"
                            + t.Lexeme
                            + "' at "
                            + t.Filename
                            + ":"
                            + t.Line
                            + ":"
                            + t.Column
                    );
                }
            }

            Parser p = new(tokens, e);
            Node root = p.Parse();

            if (printAST)
            {
                printNode(root);
            }

            e.ReportAll();
            if (e.HasErrors)
            {
                throw new InvalidOperationException(
                    $"The source has {e.Items.Count(i => i.Severity == Severity.Error)} errors and cannot be compiled."
                );
            }

            int warningsCount = e.Items.Count(i => i.Severity == Severity.Warning);
            if (warningsCount > 0)
            {
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine($"Compiling with {warningsCount} warnings.");
                Console.ForegroundColor = ConsoleColor.Gray;
            }

            Compiling.Compiler c = new(root);
            return c.Compile();
        }

        private static void writeIndent(int n)
        {
            Console.Write(new string(' ', n));
        }

        private static void printNode(Node root, int depth = 0)
        {
            writeIndent(depth);
            switch (root)
            {
                case Document d:
                    Console.WriteLine("Document");
                    printNode(d.Expression, depth + 1);
                    break;
                case LetIn l:
                    Console.WriteLine("Let In");
                    foreach (KeyValuePair<string, Node> def in l.Definitions)
                    {
                        writeIndent(depth + 1);
                        Console.WriteLine("Definition " + def.Key);
                        printNode(def.Value, depth + 2);
                    }
                    printNode(l.Expression, depth + 1);
                    break;
                case DefinitionReference r:
                    Console.WriteLine("DefinitionReference " + r.Definition);
                    break;
                case Application a:
                    Console.WriteLine("Application");
                    printNode(a.Function, depth + 1);
                    printNode(a.Argument, depth + 1);
                    break;
                case SyntaxError:
                    Console.WriteLine("Syntax Error");
                    break;
                case Lambda l:
                    Console.WriteLine("Lambda");
                    printNode(l.Body, depth + 1);
                    break;
                case Variable v:
                    Console.WriteLine($"Variable {v.DeBruijn}");
                    break;
                case Number n:
                    Console.WriteLine($"Number '{n.Value}'");
                    break;
                default:
                    Console.WriteLine(root.GetType().Name);
                    break;
            }
        }
    }
}