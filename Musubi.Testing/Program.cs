using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#define -> .
#include ~/.musubi/stdlib.mbim
head (cons 15 (: 2 (: 3 nil)))
""",
        "main.mbi",
        true
    );
}
catch (InvalidOperationException e)
{
    Console.WriteLine(e.Message);
    return 1;
}
File.WriteAllText("compiled.c", compiled);

return 0;
