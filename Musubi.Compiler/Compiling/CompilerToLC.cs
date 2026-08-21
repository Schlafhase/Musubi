using Musubi.Compiler.Nodes;

namespace Musubi.Compiler.Compiling
{
    public class CompilerToLC(Node root)
    {
        public string Compile()
        {
            return compileNode(root, 0, 0);
        }

        private string compileNode(Node n, int indentation, int lambdaDepth)
        {
            return n switch
            {
                Application a =>
                    $"({compileNode(a.Function, indentation, lambdaDepth)} {compileNode(a.Argument, indentation, lambdaDepth)})",
                DefinitionReference r => r.Definition,
                Document d => compileNode(d.Expression, indentation, lambdaDepth),
                Lambda l =>
                    $"\\{l.CapturedVariable}.\n{makeIndentation(indentation) + compileNode(l.Body, indentation + 2, lambdaDepth + 1)}",
                LetIn li => "("
                    + string.Concat(li.Definitions.Select(d => $"\\{d.Key}."))
                    + "\n"
                    + makeIndentation(indentation + 2)
                    + compileNode(li.Expression, indentation + 2, lambdaDepth)
                    + "\n"
                    + ") "
                    + string.Join(
                        ' ',
                        li.Definitions.Select(d => "(" + compileNode(d.Value, indentation, lambdaDepth) + ")")
                    ),
                Number no => no.ChurchEncoded ? churchNumeral(no.Value) : scottNumeral(no.Value),
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