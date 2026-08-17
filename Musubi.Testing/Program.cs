using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
        #include ~/.musubi/stdlib.mbim
        if true then 42 else 1 end;
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
