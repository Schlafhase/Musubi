using System.Reflection;
using System.Text;
using Musubi.Compiler.Nodes;

namespace Musubi.Compiler.Compiling
{
    public class Compiler(Node root)
    {
        private readonly StringBuilder _globalCode = new();

        private readonly Stack<string> _environment = [];
        private readonly Dictionary<string, Node> _definitions = [];

        public string Compile()
        {
            StringBuilder context = new();
            string code = compile(root, context);
            StringBuilder document = new();
            string boilerplate = "";
            Assembly assembly = Assembly.GetExecutingAssembly();
            Stream? resourceStream =
                assembly.GetManifestResourceStream("Musubi.Compiler.Compiling.boilerplate.c")
                ?? throw new Exception("boilerplate.c is missing from the assembly");
            using (StreamReader reader = new(resourceStream, Encoding.UTF8))
            {
                boilerplate = reader.ReadToEnd();
            }
            document.Append(boilerplate);
            document.Append(_globalCode.ToString());
            document.Append("int main() {");
            document.Append(context.ToString());
            document.Append(@$"printf(""%d\n"", churchToInt({code}));");
            document.Append("}");
            return document.ToString();
        }

        private string compile(Node n, StringBuilder context)
        {
            return n switch
            {
                Document d => compileDocument(d, context),
                AliasReference r => compile(_definitions[r.Alias], context),
                Variable v => compileVariable(v, context),
                Application a => compileApplication(a, context),
                Lambda l => compileLambda(l, context),
                Number no => $"churchNumeral({no.Value})",
                True => "churchBoolean(1)",
                False => "churchBoolean(0)",
                _ => throw new NotImplementedException(n.GetType().Name),
            };
        }

        private string compileDocument(Document d, StringBuilder context)
        {
            foreach (Alias def in d.Definitions)
            {
                _definitions.Add(def.Name, def.Value);
            }
            return compile(d.Expression, context);
        }

        private string compileVariable(Variable v, StringBuilder _)
        {
            return $"env->{v.Name}";
        }

        private string compileApplication(Application a, StringBuilder context)
        {
            return $"invoke({compile(a.Function, context)}, {compile(a.Argument, context)})";
        }

        private string compileLambda(Lambda l, StringBuilder context)
        {
            StringBuilder innerContext = new();
            _environment.Push(l.CapturedVariable);
            string body = compile(l.Body, innerContext);
            _environment.Pop();

            // define environment for this lambda
            _globalCode.Append("typedef struct {");
            foreach (string variable in _environment)
            {
                _globalCode.Append($"Lambda *{variable};");
            }
            _globalCode.Append($"Lambda *{l.CapturedVariable};");
            _globalCode.Append($"}} e{l.Id};");

            // define function
            _globalCode.Append($"Lambda *l{l.Id}(void *raw_env, Lambda *{l.CapturedVariable}) {{");
            _globalCode.Append($"e{l.Id} *env = raw_env;");
            _globalCode.Append($"env->{l.CapturedVariable} = {l.CapturedVariable};");
            _globalCode.Append(innerContext.ToString());
            _globalCode.Append($"return {body};");
            _globalCode.Append("}");

            // set up the lambda for the actual return value
            context.Append($"Lambda *_l{l.Id} = malloc(sizeof(Lambda));");
            context.Append($"_l{l.Id}->fn = &l{l.Id};");
            context.Append($"e{l.Id} *_e{l.Id} = malloc(sizeof(e{l.Id}));");
            foreach (string variable in _environment)
            {
                context.Append($"_e{l.Id}->{variable} = env->{variable};");
            }
            context.Append($"_l{l.Id}->env = _e{l.Id};");

            return $"_l{l.Id}";
        }
    }
}
