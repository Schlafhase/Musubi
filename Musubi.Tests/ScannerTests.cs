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
    }
}
