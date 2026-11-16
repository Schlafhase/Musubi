using System.Xml;
using Musubi.Compiler;
using Musubi.Compiler.Scanning;

namespace Musubi.Tests
{
    [TestFixture]
    public class ScannerTests
    {
        private Errors _errors;

        [SetUp]
        public void SetUp()
        {
            _errors = new();
        }

        private ScannerOut getResult(string source, string filename)
        {
            MusubiSource s = new(source, filename);
            return s.ScanTokens(_errors);
        }

        [Test]
        public void TestEmpty()
        {
            ScannerOut result = getResult("", "empty.mbi");
            Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(new[] { TokenType.EOF }));
        }

        [Test]
        public void TestNewline()
        {
            ScannerOut result = getResult("\n", "newline.mbi");
            Assert.That(result.Tokens.Select(t => t.Type), Is.EqualTo(new[] { TokenType.EOF }));

            Assert.That(
                result.Tokens.Select(t => t.LeadingTrivia.FirstOrDefault()).FirstOrDefault(),
                Is.TypeOf<Newline>()
            );
        }

        /// <summary>
        /// Tests a small module structure including some literals
        /// </summary>
        [Test]
        public void TestModule()
        {
            ScannerOut result = getResult(
                """
                let
                    a := 123,
                    b := "hello",
                    c := 'c',
                    d := someId,
                    ns := 10s,
                    nc := 15c,
                    nb := 17b,
                in
                """,
                "module.mbim"
            );

            using (Assert.EnterMultipleScope())
            {
                Assert.That(
                    result.Tokens.Select(t => t.Type),
                    Is.EqualTo(
                        new[]
                        {
                            TokenType.Let,
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.Number,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.String,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.Char,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.Identifier,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.ScottNumber,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.ChurchNumber,
                            TokenType.ListSeparator,
                            //
                            TokenType.Identifier,
                            TokenType.Definition,
                            TokenType.BinaryNumber,
                            TokenType.ListSeparator,
                            //
                            TokenType.In,
                            TokenType.EOF,
                        }
                    )
                );
                Assert.That(result.Tokens.Any(t => t.Literal is 123));
                Assert.That(result.Tokens.Any(t => t.Literal is "hello"));
                Assert.That(result.Tokens.Any(t => t.Literal is 'c'));
                Assert.That(result.Tokens.Any(t => t.Literal is 10));
                Assert.That(result.Tokens.Any(t => t.Literal is 15));
                Assert.That(result.Tokens.Any(t => t.Literal is 17));
            }
        }

        [Test]
        public void TestTrivia()
        {
            ScannerOut result = getResult(
                $"""

                ; bla1
                   {"\t"} ; bla2

                ; bla3

                a ; bla4
                ;bla5

                 b ; bla6
                ;bla7

                ; bla8


                """,
                "triviaTest.mbi"
            );

            Assert.That(result.Tokens, Has.Count.EqualTo(3));

            using (Assert.EnterMultipleScope())
            {
                // look at trivia for `a`
                Assert.That(
                    result.Tokens[0].LeadingTrivia,
                    Is.EqualTo(
                        new List<Trivia>
                        {
                            new Newline(),
                            //
                            new Comment(" bla1"),
                            new Newline(),
                            //
                            new Whitespace("   \t "),
                            new Comment(" bla2"),
                            new Newline(),
                            //
                            new Newline(),
                            //
                            new Comment(" bla3"),
                            new Newline(),
                            //
                            new Newline(),
                        }
                    )
                );
                Assert.That(
                    result.Tokens[0].TrailingTrivia,
                    Is.EqualTo(
                        new List<Trivia>
                        {
                            new Whitespace(" "),
                            new Comment(" bla4"),
                            new Newline(),
                        }
                    )
                );

                // look at trivia for `b`
                Assert.That(
                    result.Tokens[1].LeadingTrivia,
                    Is.EqualTo(
                        new List<Trivia>
                        {
                            new Comment("bla5"),
                            new Newline(),
                            //
                            new Newline(),
                            //
                            new Whitespace(" "),
                        }
                    )
                );
                Assert.That(
                    result.Tokens[1].TrailingTrivia,
                    Is.EqualTo(
                        new List<Trivia>
                        {
                            new Whitespace(" "),
                            new Comment(" bla6"),
                            new Newline(),
                        }
                    )
                );

                // test trivia for EOF token
                Assert.That(
                    result.Tokens[2].LeadingTrivia,
                    Is.EqualTo(
                        new List<Trivia>
                        {
                            new Comment("bla7"),
                            new Newline(),
                            //
                            new Newline(),
                            //
                            new Comment(" bla8"),
                            new Newline(),
                            //
                            new Newline(),
                        }
                    )
                );
            }
        }

        [Test]
        public void TestDirectives()
        {
            ScannerOut result = getResult(
                """
                #once
                #include stdlib
                #define -> a b
                c
                """,
                "directives.mbi"
            );

            Assert.That(
                result.Tokens.Select(t => t.Type),
                Is.EqualTo(
                    new TokenType[]
                    {
                        TokenType.IncludeOnce,
                        //
                        TokenType.Include,
                        TokenType.Filename,
                        //
                        TokenType.Define,
                        TokenType.Identifier,
                        TokenType.Identifier,
                        TokenType.Identifier,
                        //
                        TokenType.Identifier,
                    }
                )
            );
        }
    }
}
