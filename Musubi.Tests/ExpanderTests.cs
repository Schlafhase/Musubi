using System.Text;
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
            // inside basic.mbi
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
        using (Assert.EnterMultipleScope())
        {
            Assert.That(result.Tokens[1].Literal, Is.EqualTo("basic.mbi"));
            Assert.That(result.Tokens[12].Literal, Is.EqualTo("->"));
        }
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
            TokenType.Define,
            TokenType.Identifier, // world
            TokenType.String, // "world"
            //
            TokenType.EndInclude,
            //
            TokenType.EndInclude,
            //
            TokenType.BeginMacro,
            TokenType.String, // "world" from macro 'world'
            TokenType.EndMacro,
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
            Assert.That(result.Context.ErrorLogger.HasErrors);
            Assert.That(result.Context.ErrorLogger.Items, Has.Count.EqualTo(1));
        }
        Assert.That(result.Context.ErrorLogger.Items[0].Kind, Is.EqualTo(Errors.CircularInclude));
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
            Assert.That(result.Context.ErrorLogger.Items, Has.Count.EqualTo(1));
        }
        Assert.That(result.Context.ErrorLogger.Items[0].Kind, Is.EqualTo(Errors.DuplicateInclude));
    }

    [Test]
    public void TestNewlineAfterDirectives()
    {
        ExpanderOut result1 = getResult(
            """
            #define
            bla blibb

            #include
            blubb.mbi
            0
            """,
            "newlinestuff.mbi"
        );

        TokenType[] expected1 =
        [
            TokenType.Define,
            TokenType.Identifier,
            TokenType.Identifier,
            //
            TokenType.Include,
            TokenType.Number,
            TokenType.EOF,
        ];

        using (Assert.EnterMultipleScope())
        {
            Assert.That(result1.Tokens.Select(t => t.Type), Is.EqualTo(expected1));
            Assert.That(
                from i in result1.Context.ErrorLogger.Items
                select i.Kind,
                Is.EqualTo(new ErrorKind[] { Errors.NewlineInDirective, Errors.NewlineInDirective })
            );
        }
    }

    [Test]
    public void TestRoundtrip()
    {
        ExpanderOut result = getResult(
            SharedValues.BinaryArithmeticsSource,
            "binaryArithmetics.mbim"
        );

        StringBuilder reconstructed = new();

        for (int i = 0; i < result.Tokens.Count; i++)
        {
            Token t = result.Tokens[i];
            reconstructed.Append(string.Concat(t.LeadingTrivia.Select(tr => tr.ToString())));
            if (t.Type is TokenType.BeginMacro)
            {
                reconstructed.Append(t.Literal as string);
                while (i + 1 < result.Tokens.Count)
                {
                    i++;
                    if (result.Tokens[i].Type is TokenType.EndMacro)
                    {
                        t = result.Tokens[i];
                        break;
                    }
                }
            }
            else if (t.Type is TokenType.BeginInclude)
            {
                reconstructed.Append(t.Lexeme);
                while (i + 1 < result.Tokens.Count)
                {
                    i++;
                    if (result.Tokens[i].Type is TokenType.EndInclude)
                    {
                        t = result.Tokens[i];
                        break;
                    }
                }
            }
            else
            {
                reconstructed.Append(t.Lexeme);
            }
            reconstructed.Append(string.Concat(t.TrailingTrivia.Select(tr => tr.ToString())));
        }

        Assert.That(reconstructed.ToString(), Is.EqualTo(SharedValues.BinaryArithmeticsSource));
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
                #define world "world"
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