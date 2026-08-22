using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#define -> .
#include stdlib/recursion.mbim
        let
            S := \n.\z.\s.s n,
            pred := \n.n 0s (\n -> n),
            add := rec (\add.\a -> \b ->
                a b (S (add (pred a) b))),
            fib := rec (\fib.\n -> n 1s (\p -> add (fib p) (fib (pred p))))
        in
            fib 5
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
