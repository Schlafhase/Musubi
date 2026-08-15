using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(@"15");
}
catch (InvalidOperationException e)
{
    Console.WriteLine(e.Message);
    return 1;
}
File.WriteAllText("compiled.c", compiled);

return 0;
