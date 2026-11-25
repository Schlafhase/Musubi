using Musubi.Compiler;
using Musubi.Compiler.CST;
using Musubi.Compiler.Expanding;
using Musubi.Compiler.Parsing;
using Musubi.Compiler.Scanning;

namespace Musubi.Tests
{
    [TestFixture]
    public class ParserTests
    {
        private ErrorLogger _errors;

        [SetUp]
        public void SetUp()
        {
            _errors = new();
        }

        private ParserOut getResult(string source, string filename)
        {
            MusubiSource s = new(source, new FileInfo(filename));
            CompilationContext c = new(s, _errors);
            return c.ScanTokens().Expand().Parse();
        }

        [Test]
        public void TestFac()
        {
            ParserOut result = getResult(SharedValues.FacSourceNoIncludes, "factorial.mbi");

#pragma warning disable NUnit2010 // Use EqualConstraint for better assertion messages in case of failure
            Assert.That(
                result.CST
                    is Document
                    {
                        Start.Type: TokenType.Let,
                        End.Type: TokenType.EOF,
                        Children: [
                            LetIn
                            {
                                Children: [
                                    Definition
                                    {
                                        Children: [
                                            Identifier,
                                            Application
                                            {
                                                Children: [
                                                    Identifier { Name: "fac" },
                                                    Parentheses
                                                    {
                                                        Children: [
                                                            Lambda { CapturedVariable: "fac" },
                                                        ]
                                                    },
                                                ]
                                            },
                                        ]
                                    },
                                    Application,
                                ]
                            },
                        ]
                    }
            );
#pragma warning restore NUnit2010 // Use EqualConstraint for better assertion messages in case of failure
        }
    }
}
