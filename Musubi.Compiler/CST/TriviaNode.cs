using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.CST
{
    public class TriviaNode(Trivia trivia) : CSTNode
    {
        public required Trivia Trivia = trivia;
    }
}
