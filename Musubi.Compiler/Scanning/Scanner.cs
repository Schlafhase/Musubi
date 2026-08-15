namespace Musubi.Compiler.Scanning
{
    public class Scanner(string source, Errors errors)
    {
        private readonly string _source = source;
        private readonly List<Token> _tokens = [];

        private int _start;
        private int _current;
        private int _line = 1;
        private int _column = 1;

        private static readonly Dictionary<string, TokenType> _keywords = new()
        {
            { "true", TokenType.True },
            { "false", TokenType.False },
        };

        public List<Token> ScanTokens()
        {
            while (!endReached())
            {
                _start = _current;
                scanToken();
            }

            _tokens.Add(new(TokenType.EOF, "", _line, _column));
            return _tokens;
        }

        private void scanToken()
        {
            char c = advance();
            switch (c)
            {
                case '(':
                    addToken(TokenType.LeftParen);
                    break;
                case ')':
                    addToken(TokenType.RightParen);
                    break;
                case '.':
                    addToken(TokenType.Dot);
                    break;
                case '\\':
                case 'λ':
                    addToken(TokenType.Lambda);
                    break;
                case ' ':
                case '\r':
                case '\t':
                    break;
                case '\n':
                    _line++;
                    _column = 1;
                    break;
                default:
                    if (digit(c))
                    {
                        number();
                    }
                    else if (alphabetic(c))
                    {
                        identifier();
                    }
                    else
                    {
                        errors.Report("Unexpected character '" + c + "'", _line, _column - 1); // column - 1 because column refers to the column after advancing
                    }
                    break;
            }
        }

        private void number()
        {
            while (digit(peek()))
            {
                advance();
            }
            addToken(TokenType.Number);
        }

        private void identifier()
        {
            while (alphaNumeric(peek()))
            {
                advance();
            }

            string text = _source[_start.._current];
            if (_keywords.TryGetValue(text, out TokenType type))
            {
                addToken(type);
            }
            else
            {
                addToken(TokenType.Identifier);
            }
        }

        private char advance()
        {
            _column++;
            return _source[_current++];
        }

        private char peek(int offset = 0)
        {
            if (_current + offset >= _source.Length)
            {
                return '\0';
            }
            return _source[_current + offset];
        }

        private bool alphabetic(char c)
        {
            return char.ToLower(c) is >= 'a' and <= 'z';
        }

        private bool digit(char c)
        {
            return c is >= '0' and <= '9';
        }

        private bool alphaNumeric(char c)
        {
            return alphabetic(c) || digit(c);
        }

        private void addToken(TokenType type)
        {
            string text = _source[_start.._current];
            _tokens.Add(new(type, text, _line, _column - (_current - _start)));
        }

        private bool endReached()
        {
            return _current >= _source.Length;
        }
    }
}
