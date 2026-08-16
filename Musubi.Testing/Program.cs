using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
        include
            /home/Linus/.musubi/stdlib.mbim;
        in
        let
            rec' := \self.\n.
                if IsZero n then
                    1
                else
                    + n (self (pred n))
                end;
            rec := Z (\x.x);
        in
            rec 5;
        """,
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
