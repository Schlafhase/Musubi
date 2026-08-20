using Musubi.Compiler;

string compiled;
try
{
    compiled = MusubiCompiler.Compile(
        """
#include stdlib
#include stdlib/lazyLists.mbim
let
  numbers := grab 5 naturals
in
  toString<List<ints>> (numbers)
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
