using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#include stdlib
take 5 "Hello World!"
""",
        "main.mbi",
        true
    );
}
catch (AbandonedMutexException e)
{
    Console.WriteLine(e.Message);
    return 1;
}
File.WriteAllText("compiled.c", compiled);

return 0;