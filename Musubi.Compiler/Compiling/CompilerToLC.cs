using Musubi.Compiler.Nodes;

namespace Musubi.Compiler.Compiling
{
    public class CompilerToLC(Node root) : ICompiler
    {
        public string Compile()
        {
            return compileNode(root);
        }

        private string compileNode(Node n)
        {
            return n switch
            {
                Application a => $"({compileNode(a.Function)} {compileNode(a.Argument)})",
                DefinitionReference r => r.Definition,
                Document d => compileNode(d.Expression),
                Lambda l => $"(\\{l.CapturedVariable}.{compileNode(l.Body)})",
                LetIn li => "("
                    + string.Concat(li.Definitions.Select(d => $"\\{d.Key}."))
                    + compileNode(li.Expression)
                    + ") "
                    + string.Join(
                        ' ',
                        li.Definitions.Select(d => "(" + compileNode(d.Value) + ")")
                    ),
                Number no => no.Encoding switch
                {
                    NumeralEncoding.Scott => scottNumeral(no.Value),
                    NumeralEncoding.Church => churchNumeral(no.Value),
                    _ => throw new NotImplementedException(
                        $"The '{no.Encoding}' encoding is not implemented"
                    ),
                },
                Variable v => $"{v.ReferencedVariable}",
                _ => throw new NotImplementedException(),
            };
        }

        private string churchNumeral(int n)
        {
            string result = @"\f.\x.";
            for (int i = 0; i < n; i++)
            {
                result += "f (";
            }
            result += "x";
            for (int i = 0; i < n; i++)
            {
                result += ")";
            }
            return result;
        }

        private string scottNumeral(int n)
        {
            if (n == 0)
            {
                return @"(\z.\s.z)";
            }
            else
            {
                return @$"(\z.\s.s {scottNumeral(n - 1)})";
            }
        }

        private static string makeIndentation(int n)
        {
            return new string(' ', n);
        }
    }
}
