using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#include stdlib
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
