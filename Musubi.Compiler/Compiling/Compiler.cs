using System.Reflection;
using System.Text;
using Musubi.Compiler.Nodes;

namespace Musubi.Compiler.Compiling
{
    internal struct CompilingContext
    {
        public StringBuilder CodeBuilder = new();
        public Dictionary<string, string> DefinitionToCode = [];
        public int VariableCount;

        public CompilingContext() { }
    }

    public class Compiler(Node root)
    {
        private readonly StringBuilder _globalCode = new();

        public string Compile()
        {
            CompilingContext context = new();
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
            document.Append(context.CodeBuilder.ToString());
            document.Append($"Lambda *result = {code};");
            document.Append("printf(\"printing now\\n\");");
            document.Append("printScottString(result);");
            document.Append("}");
            return document.ToString();
        }

        private string compile(Node n, CompilingContext context)
        {
            return n switch
            {
                Document d => compileDocument(d, context),
                LetIn let => compileLetIn(let, context),
                DefinitionReference r => compileDefinitionReference(r, context),
                Variable v => compileVariable(v, context),
                Application a => compileApplication(a, context),
                Lambda l => compileLambda(l, context),
                Number no => no.ChurchEncoded
                    ? $"churchNumeral({no.Value})"
                    : $"scottNumeral({no.Value})",
                _ => throw new NotImplementedException(n.GetType().Name),
            };
        }

        private string compileDocument(Document d, CompilingContext context)
        {
            return compile(d.Expression, context);
        }

        private string compileLetIn(LetIn let, CompilingContext context)
        {
            CompilingContext innerContext = new()
            {
                CodeBuilder = context.CodeBuilder,
                DefinitionToCode = context.DefinitionToCode.ToDictionary(), // copy
            };
            // define a global function for each definition
            foreach (KeyValuePair<string, Node> def in let.Definitions)
            {
                string id = Guid.NewGuid().ToString().Replace("-", "");
                CompilingContext definitionContext = new()
                {
                    DefinitionToCode = innerContext.DefinitionToCode,
                };
                string compiledBody = compile(def.Value, definitionContext);
                _globalCode.AppendLine();
                _globalCode.AppendLine($"// {def.Key}");
                // Caching after first evaluation
                _globalCode.Append($"Lambda *d{id}cached = NULL;");
                // Actual definition (lambda that returns the body)
                _globalCode.Append($"Lambda *d{id}() {{");
                _globalCode.Append(definitionContext.CodeBuilder.ToString());
                _globalCode.Append(
                    $"if (d{id}cached == NULL) d{id}cached = {compiledBody}; return d{id}cached;}}"
                );
                innerContext.DefinitionToCode[def.Key] = $"d{id}()";
            }
            return compile(let.Expression, innerContext);
        }

        private string compileDefinitionReference(DefinitionReference r, CompilingContext context)
        {
            return context.DefinitionToCode[r.Definition];
        }

        private string compileVariable(Variable v, CompilingContext _)
        {
            return $"env->v{v.DeBruijn}";
        }

        private string compileApplication(Application a, CompilingContext context)
        {
            return $"invoke({compile(a.Function, context)}, {compile(a.Argument, context)})";
        }

        private string compileLambda(Lambda l, CompilingContext context)
        {
            CompilingContext innerContext = new()
            {
                DefinitionToCode = context.DefinitionToCode,
                VariableCount = context.VariableCount + +1,
            };
            string body = compile(l.Body, innerContext);

            // define environment for this lambda
            _globalCode.Append("typedef struct {");
            for (int i = 0; i <= context.VariableCount; i++)
            {
                _globalCode.Append($"Lambda *v{i};");
            }
            _globalCode.Append($"}} e{l.Id};");

            // define function
            _globalCode.Append($"Lambda *l{l.Id}(void *raw_env, Lambda *arg) {{");
            _globalCode.Append($"e{l.Id} *env = raw_env;");
            _globalCode.Append("env->v0 = arg;");
            _globalCode.Append(innerContext.CodeBuilder.ToString());
            _globalCode.Append($"return {body};");
            _globalCode.Append("}");

            // set up the lambda for the actual return value
            context.CodeBuilder.Append($"Lambda *_l{l.Id} = malloc(sizeof(Lambda));");
            context.CodeBuilder.Append($"_l{l.Id}->fn = &l{l.Id};");
            context.CodeBuilder.Append($"e{l.Id} *_e{l.Id} = malloc(sizeof(e{l.Id}));");
            for (int i = 0; i < context.VariableCount; i++)
            {
                context.CodeBuilder.Append($"_e{l.Id}->v{i + 1} = env->v{i};");
            }
            context.CodeBuilder.Append($"_l{l.Id}->env = _e{l.Id};");

            return $"_l{l.Id}";
        }
    }
}