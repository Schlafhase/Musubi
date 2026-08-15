using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing
{
    public class Parser(List<Token> tokens, Errors errors)
    {
        private int _current;

        public Node Parse()
        {
            return parseToken();
        }

        private Node parseToken()
        {
            return expression();
        }

        // expression: atom atom*
        private Node expression()
        {
            Node left = atom();

            while (
                check(
                    TokenType.Lambda,
                    TokenType.Identifier,
                    TokenType.LeftParen,
                    TokenType.True,
                    TokenType.False,
                    TokenType.Number
                )
            )
            {
                Node right = atom();
                left = new Application() { Function = left, Argument = right };
            }

            return left;
        }

        // atom: lambda | variable | "(" expression ")" | true | false
        private Node atom()
        {
            switch (peek().Type)
            {
                case TokenType.Lambda:
                    return lambda();
                case TokenType.Identifier:
                    string id = identifier();
                    return new Variable() { Name = id };
                case TokenType.LeftParen:
                    advance();
                    Node inner = expression();
                    expect("closing parenthesis", TokenType.RightParen);
                    return inner;
                case TokenType.Number:
                    advance();
                    return new Number() { Value = int.Parse(previous().Lexeme) };
                case TokenType.True:
                    advance();
                    return new True();
                case TokenType.False:
                    advance();
                    return new False();
                default:
                    errors.ReportUnexpected(peek(), "expression");
                    return new Error();
            }
        }

        // lambda: ("\" | "λ") identifier "." expression
        private Node lambda()
        {
            if (expect(TokenType.Lambda))
            {
                string captured = identifier();
                if (expect(TokenType.Dot))
                {
                    Node body = expression();
                    return new Lambda() { CapturedVariable = captured, Body = body };
                }
            }
            return new Error();
        }

        private string identifier()
        {
            expect(TokenType.Identifier);
            return previous().Lexeme;
        }

        // helpers

        private void advance()
        {
            _current++;
        }

        private Token previous()
        {
            return tokens[_current - 1];
        }

        private Token peek()
        {
            return tokens[_current];
        }

        private bool check(params TokenType[] types)
        {
            if (endReached())
            {
                return false;
            }
            return types.Contains(peek().Type);
        }

        private bool match(params TokenType[] types)
        {
            if (check(types))
            {
                advance();
                return true;
            }

            return false;
        }

        private bool expect(params TokenType[] types)
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.ReportUnexpected(next, types);
            return false;
        }

        private bool expect(string expected, params TokenType[] types)
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.ReportUnexpected(next, expected);
            return false;
        }

        private bool endReached()
        {
            return peek().Type == TokenType.EOF;
        }
    }
}
