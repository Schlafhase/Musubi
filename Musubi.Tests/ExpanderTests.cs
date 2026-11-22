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

    [Test]
    public void TestNestedInclude()
    {
        ExpanderOut result = getResult(SharedValues.NestedIncludeSource, "nestedInclude.mbi");

        TokenType[] expected =
        [
            TokenType.BeginInclude,
            // inside nested.mbi
            TokenType.BeginInclude,
            // inside nested2.mbi
            TokenType.String, // "hello"
            TokenType.Identifier, // @
            //
            TokenType.EndInclude,
            //
            TokenType.EndInclude,
            //
            TokenType.String, // "world"
            //
            TokenType.EOF,
        ];

        Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(expected));
    }

    [Test]
    public void TestCircularInclude()
    {
        ExpanderOut result = getResult(
            fileResolver("circular1.mbi", "", out string resolvedTo),
            resolvedTo
        );

        TokenType[] expected =
        [
            TokenType.BeginInclude,
            //
            TokenType.BeginInclude, // empty because of the circular include
            TokenType.EndInclude,
            //
            TokenType.EndInclude,
            //
            TokenType.EOF,
        ];

        using (Assert.EnterMultipleScope())
        {
            Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(expected));
            Assert.That(result.ErrorLogger.HasErrors);
            Assert.That(result.ErrorLogger.Items, Has.Count.EqualTo(1));
        }
        Assert.That(result.ErrorLogger.Items[0].Kind, Is.EqualTo(Errors.CircularInclude));
    }

    [Test]
    public void TestIncludeOnce()
    {
        ExpanderOut result = getResult(
            """
            #include once.mbi #include once.mbi
            #include includingOnce.mbi
            """,
            "testOnce.mbi"
        );

        TokenType[] expected =
        [
            TokenType.BeginInclude, // include once.mbi
            TokenType.IncludeOnce,
            TokenType.Identifier, // something
            TokenType.EndInclude,
            //
            TokenType.BeginInclude, // include once.mbi (emit warning)
            TokenType.EndInclude,
            //
            TokenType.BeginInclude, // include includingOnce.mbi
            TokenType.BeginInclude, // include once.mbi (no warning)
            TokenType.EndInclude,
            TokenType.Identifier, // somethingElse
            TokenType.EndInclude,
            //
            TokenType.EOF,
        ];

        using (Assert.EnterMultipleScope())
        {
            Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(expected));
            Assert.That(result.ErrorLogger.Items, Has.Count.EqualTo(1));
        }
        Assert.That(result.ErrorLogger.Items[0].Kind, Is.EqualTo(Errors.DuplicateInclude));
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
            "nested.mbi" => """
                #include nested2.mbi
                """,
            "nested2.mbi" => """
                "hello" @
                """,
            "circular1.mbi" => """
                #include circular2.mbi
                """,
            "circular2.mbi" => """
                #include circular1.mbi
                """,
            "once.mbi" => """
                #once
                something
                """,
            "includingOnce.mbi" => """
                #include once.mbi
                somethingElse
                """,
            _ => throw new FileNotFoundException(),
        };
    }
}
