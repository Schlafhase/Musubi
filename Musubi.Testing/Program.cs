using Musubi.Compiler;
using Musubi.Compiler.Nodes;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;

string source = "\\x.x a- (324\n(_(b)))()";
Errors e = new(source);
Scanner s = new(source, e);
List<Token> tokens = s.ScanTokens();

Parser p = new(tokens, e);
Node root = p.Parse();

foreach (Token token in tokens)
{
    Console.WriteLine(token.ToString());
}

Console.WriteLine("AST:");

static void writeIndent(int n)
{
    Console.Write(new string(' ', n));
}

static void printNode(Node root, int depth = 0)
{
    writeIndent(depth);
    switch (root)
    {
        case Application a:
            Console.WriteLine("Application");
            printNode(a.Function, depth + 1);
            printNode(a.Argument, depth + 1);
            break;
        case Error:
            Console.WriteLine("Syntax Error");
            break;
        case Lambda l:
            Console.WriteLine($"Lambda ({l.CapturedVariable})");
            printNode(l.Body, depth + 1);
            break;
        case Variable v:
            Console.WriteLine($"Variable '{v.Name}'");
            break;
        case Number n:
            Console.WriteLine($"Number '{n.Value}");
            break;
        case True:
            Console.WriteLine("True");
            break;
        case False:
            Console.WriteLine("False");
            break;
    }
}

printNode(root);
