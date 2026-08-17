using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
        #define -> .
        #include ~/.musubi/stdlib.mbim
        let
            x := let z := 0; in z;
            x := let z := 1; in z;
            true := false;
            sum' := \sum -> \n ->
                if IsZero n then
                    0
                else
                    + n (sum (pred n)) 
                end;
            sum := rec sum';
            sum5e := sum 5;
        in
            y;
        """,
        "main.mbi",
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
