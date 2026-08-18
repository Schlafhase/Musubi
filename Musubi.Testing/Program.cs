using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#define -> .
#include stdlib
cons 'H' (cons 'e' (cons 'l' (cons 'l' (cons 'o' (cons ' ' (cons 'W' (cons 'o' (cons 'r' (cons 'l' (cons 'd' (cons '!' nil)))))))))))
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
