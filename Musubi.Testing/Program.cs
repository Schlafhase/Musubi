using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
        let
            + := 
                let S := \n.\f.\x.f(n f x);
                in \a.\b.a S b;
        in
                (+ 20 (+ 4 5));
        """,
        false
    );
}
catch (InvalidOperationException e)
{
    Console.WriteLine(e.Message);
    return 1;
}
File.WriteAllText("compiled.c", compiled);

return 0;
