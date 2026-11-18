namespace Musubi.Compiler.Scanning
{
    public readonly struct ScannerOut
    {
        public readonly List<Token> Tokens = [];
        public required ErrorLogger ErrorLogger { get; init; }

        public ScannerOut() { }
    }

    public static class MusubiSourceScanExtension
    {
        public static ScannerOut ScanTokens(this MusubiSource source, ErrorLogger errors)
        {
            Scanner s = new(source, errors);
            return s.ScanTokens();
        }
    }

    public class Scanner(MusubiSource source, ErrorLogger errors) : Consumer<char>
    {
        private readonly string _source = source.Source;
        private readonly List<Token> _tokens = [];

        private int _start;
        private int _current;
        private int _line = 1;
        private int _column = 1;

        // attempt to parse next token as filename
        private bool _expectFilename;

        private readonly List<Trivia> _currentLeadingTrivia = [];
        private readonly List<Trivia> _currentTrailingTrivia = [];
        private bool _addingToLeadingTrivia = true;
        private List<Trivia> _currentTrivia =>
            _addingToLeadingTrivia ? _currentLeadingTrivia : _currentTrailingTrivia;

        private static readonly Dictionary<string, TokenType> _keywords = new()
        {
            { "let", TokenType.Let },
            { "in", TokenType.In },
            { "infix", TokenType.Infix },
            { "infixl", TokenType.Infixl },
            { "infixr", TokenType.Infixr },
            { ":=", TokenType.Definition },
        };

        private static readonly HashSet<char> _disallowedIdentifierChars =
        [
            ' ',
            '\t',
            '\n',
            '\r',
            '(',
            ')',
            '.',
            '\\',
            'λ',
            ';',
            ',',
            '\0',
        ];

        private static readonly HashSet<char> _allowedFilenameChars =
        [
            '_',
            '-',
            '.',
            '/',
            '\\',
            '~',
        ];

        public ScannerOut ScanTokens()
        {
            while (!endReached())
            {
                _start = _current;
                scanToken();
            }
            _column++;
            _start = _current;
            addToken(TokenType.EOF);

            ScannerOut r = new() { ErrorLogger = errors };
            r.Tokens.AddRange(_tokens);
            return r;
        }

        private void scanToken()
        {
            char c = advance();
            switch (c)
            {
                // trivia
                case ';':
                    int start = _current;
                    while (!endReached() && advance() != '\n') { }
                    string comment = _source[start..(_current - 1)];
                    _currentTrivia.Add(new Comment(comment));
                    if (peek(-1) == '\n')
                    {
                        goto newline;
                    }
                    break;
                case '\r':
                    break;
                case ' ':
                case '\t':
                    if (_currentTrivia.Count >= 1 && _currentTrivia[^1] is Whitespace ws)
                    {
                        _currentTrivia[^1] = ws with { Text = ws.Text + c };
                    }
                    else
                    {
                        _currentTrivia.Add(new Whitespace(c.ToString()));
                    }
                    break;
                case '\n':
                    newline:
                    _currentTrivia.Add(new Newline());
                    _addingToLeadingTrivia = true;
                    _line++;
                    _column = 1;
                    break;

                // filename
                case char when _expectFilename:
                    filename();
                    break;

                // simple tokens
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
                case ',':
                    addToken(TokenType.ListSeparator);
                    break;

                // directives
                case '#':
                    if (checkNextAndAdvance("include"))
                    {
                        addToken(TokenType.Include);
                        _expectFilename = true;
                    }
                    else if (checkNextAndAdvance("once"))
                    {
                        addToken(TokenType.IncludeOnce);
                    }
                    else if (checkNextAndAdvance("define"))
                    {
                        addToken(TokenType.Define);
                    }
                    break;

                // literal shenanigans
                case char when digit(c):
                    number();
                    break;
                case char when c == '\'':
                    characterOrIdentifier();
                    break;
                case char when c == '"':
                    stringLiteral();
                    break;
                // identifier
                case char when !_disallowedIdentifierChars.Contains(c):
                    identifier();
                    break;

                default:
                    errors.AddDiagnostic(
                        Errors.UnexpectedCharacter,
                        c,
                        source.Filename,
                        _line,
                        _column - 1
                    ); // column - 1 because column refers to the column after advancing
                    break;
            }
        }

        private void stringLiteral()
        {
            string value = "";
            while (peek() is not ('"' or '\n' or '\0'))
            {
                char next = advance();
                if (next == '\\')
                {
                    switch (advance())
                    {
                        case '\\':
                            next = '\\';
                            break;
                        case '"':
                            next = '"';
                            break;
                        case 'n':
                            next = '\n';
                            break;
                        case 't':
                            next = '\t';
                            break;
                        default:
                            errors.AddDiagnostic(
                                Errors.UnknownEscapeSequence,
                                source.Filename,
                                _line,
                                _column
                            );
                            break;
                    }
                }
                value += next;
            }
            if (peek() == '"')
            {
                advance();
                addToken(TokenType.String, value);
            }
            else
            {
                errors.AddDiagnostic(
                    Errors.UnexpectedText,
                    "\"",
                    peek() == '\n' ? "\\n" : "EOF",
                    source.Filename,
                    _line,
                    _column
                );
            }
        }

        private void characterOrIdentifier()
        {
            // try to parse character literal
            char value = advance();
            if (value == '\\')
            {
                switch (advance())
                {
                    case '\\':
                        value = '\\';
                        break;
                    case '\'':
                        value = '\'';
                        break;
                    case 'n':
                        value = '\n';
                        break;
                    case 't':
                        value = '\t';
                        break;
                    default:
                        errors.AddDiagnostic(
                            Errors.UnknownEscapeSequence,
                            source.Filename,
                            _line,
                            _column,
                            2
                        );
                        break;
                }
            }
            if (advance() == '\'')
            {
                addToken(TokenType.Char, value);
            }
            else
            {
                identifier();
            }
        }

        private void number()
        {
            while (digit(peek()))
            {
                advance();
            }
            int value = int.Parse(currentLexeme());
            if (peek() == 'c')
            {
                advance();
                addToken(TokenType.ChurchNumber, value);
                return;
            }
            else if (peek() == 's')
            {
                advance();
                addToken(TokenType.ScottNumber, value);
                return;
            }
            else if (peek() == 'b')
            {
                advance();
                addToken(TokenType.BinaryNumber, value);
                return;
            }
            addToken(TokenType.Number, value);
        }

        private string identifierString()
        {
            while (!_disallowedIdentifierChars.Contains(peek()))
            {
                advance();
            }

            return currentLexeme();
        }

        private void identifier()
        {
            string text = identifierString();
            if (_keywords.TryGetValue(text, out TokenType type))
            {
                addToken(type);
            }
            else
            {
                addToken(TokenType.Identifier, text);
            }
        }

        private void filename()
        {
            while (alphaNumeric(peek()) || _allowedFilenameChars.Contains(peek()))
            {
                advance();
            }
            if (_current == _start)
            {
                errors.AddDiagnostic(Errors.ExpectedFilename, source.Filename, _line, _column);
            }
            else
            {
                addToken(TokenType.Filename, currentLexeme());
            }
            _expectFilename = false;
        }

        private char advance()
        {
            if (endReached())
            {
                return '\0';
            }
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

        // NOTE: cannot handle newlines in parameter text
        private bool checkNextAndAdvance(string text)
        {
            if (_current + text.Length > _source.Length)
            {
                return false;
            }
            if (_source[_current..(_current + text.Length)] == text)
            {
                _column += text.Length;
                _current += text.Length;
                return true;
            }
            return false;
        }

        private string currentLexeme()
        {
            return _source[_start.._current];
        }

        private static bool alphabetic(char c) => char.ToLower(c) is >= 'a' and <= 'z';

        private static bool digit(char c) => c is >= '0' and <= '9';

        private static bool alphaNumeric(char c) => alphabetic(c) || digit(c);

        private void addToken(TokenType type, object? literal = null)
        {
            if (_tokens.Count > 0)
            {
                _tokens[^1].TrailingTrivia = [.. _currentTrailingTrivia];
            }
            _tokens.Add(
                new(type, currentLexeme(), _line, _column - (_current - _start), source.Filename)
                {
                    Literal = literal,
                    LeadingTrivia = [.. _currentLeadingTrivia],
                }
            );
            _currentLeadingTrivia.Clear();
            _currentTrailingTrivia.Clear();
            _addingToLeadingTrivia = false;
        }

        private bool endReached()
        {
            return _current >= _source.Length;
        }
    }
}
