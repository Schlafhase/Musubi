using Musubi.Compiler;
using Musubi.Compiler.Expanding;
using Musubi.Compiler.Scanning;

namespace Musubi.Tests;

[TestFixture]
public class ExpanderTests()
{
    private ErrorLogger _errors;

    [SetUp]
    public void SetUp()
    {
        _errors = new();
    }

    private ExpanderOut getResult(string source, string filename)
    {
        MusubiSource s = new(source, new FileInfo(filename));
        CompilationContext c = new(s, _errors);
        return c.ScanTokens().Expand(fileResolver);
    }

    [Test]
    public void TestDirectives()
    {
        ExpanderOut result = getResult(SharedValues.DirectivesSource, "diretives.mbi");

        TokenType[] expected =
        [
            TokenType.IncludeOnce,
            //
            TokenType.BeginInclude,
            // inside something.mbi
            TokenType.Let,
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Number,
            TokenType.In,
            //
            TokenType.EndInclude,
            //
            TokenType.Define,
            TokenType.Identifier,
            TokenType.String,
            //
            TokenType.Identifier,
            // expanded from "->"
            TokenType.BeginMacro,
            TokenType.String,
            TokenType.EndMacro,
            //
            TokenType.IncludeOnce,
            TokenType.EOF,
        ];

        Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(expected));
    }

    private static string fileResolver(string target, string relativeTo, out string resolvedTo)
    {
        resolvedTo = "/test-stubs/" + target;
        return target switch
        {
            "basic.mbi" => """
                let 
                    x := 2 
                in
                """,
            _ => throw new FileNotFoundException(),
        };
    }
}
