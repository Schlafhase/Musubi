using Musubi.Compiler;
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

            Assert.That(
                result.CST.GetStringRepresentation(),
                Is.EqualTo(
                    "Document(LetIn(Definition[fac](Application(Identifier[rec] Parentheses(Lambda[fac](Lambda[n](Application(Application(Identifier[n] NumberLiteral[1]) Parentheses(Lambda[p](InfixApplication[*](Identifier[p] Parentheses(Application(Identifier[fac] Identifier[p])))))))))))))"
                )
            );
        }
    }
}
