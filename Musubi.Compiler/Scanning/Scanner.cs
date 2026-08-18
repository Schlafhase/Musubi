namespace Musubi.Compiler.Scanning
{
    public class Scanner(
        string source,
        string filepath,
        Errors errors,
        HashSet<string>? includedFrom = null,
        HashSet<string>? includeOnceAlreadyIncluded = null,
        Dictionary<string, List<Token>>? macros = null
    )
    {
        private readonly string _source = source;
        private readonly List<Token> _tokens = [];

        private int _start;
        private int _current;
        private int _line = 1;
        private int _column = 1;

        private readonly Dictionary<string, List<Token>> _macros = macros ?? [];

        private static readonly Dictionary<string, TokenType> _keywords = new()
        {
            { "let", TokenType.Let },
            { "in", TokenType.In },
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

        private static readonly HashSet<char> _allowedFilenameChars = ['_', '-', '.', '/', '~'];

        private readonly HashSet<string> _includeOnceAlreadyIncluded =
            includeOnceAlreadyIncluded ?? [];

        public List<Token> ScanTokens()
        {
            ScanTokensInIncludedFile();
            _tokens.Add(new(TokenType.EOF, "", _line, _column + 1, filepath));
            return [.. _tokens.Where(t => t.Type != TokenType.IncludeOnce)];
        }

        public (
            List<Token> tokens,
            Dictionary<string, List<Token>> macros
        ) ScanTokensInIncludedFile()
        {
            while (!endReached())
            {
                _start = _current;
                scanToken();
            }

            return (_tokens, _macros);
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
                case ',':
                    addToken(TokenType.ListSeparator);
                    break;
                case ';':
                    while (!endReached() && peek() != '\n')
                    {
                        advance();
                    }
                    break;
                case ':':
                    if (peek() == '=')
                    {
                        advance();
                        addToken(TokenType.Definition);
                        break;
                    }
                    else
                    {
                        identifier();
                    }
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
                    else if (c == '\'')
                    {
                        // try to parse character literal
                        char value = advance();
                        if (advance() == '\'' && _disallowedIdentifierChars.Contains(peek()))
                        {
                            advance();
                            addToken(TokenType.ScottNumber, (int)value);
                        }
                        else
                        {
                            identifier();
                        }
                    }
                    else if (!_disallowedIdentifierChars.Contains(c))
                    {
                        identifier();
                    }
                    else
                    {
                        errors.Error(
                            "Unexpected character '" + c + "'",
                            filepath,
                            _line,
                            _column - 1
                        ); // column - 1 because column refers to the column after advancing
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
            addToken(TokenType.ScottNumber, value);
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
            if (text == "#define")
            {
                while (char.IsWhiteSpace(peek()))
                {
                    advance();
                }
                _start = _current;
                string name = identifierString();
                if (string.IsNullOrEmpty(name))
                {
                    errors.Error("Expected macro name", filepath, _line, _column);
                    return;
                }
                _start = _current;
                string macroSource = "";
                char c = advance();
                while (c != '\n')
                {
                    macroSource += c;
                    c = advance();
                }
                Errors macroErrors = new();
                macroErrors.FilenameToSource[$"Macro '{name}' defined in {filepath}"] = macroSource;
                Scanner macroScanner = new(
                    macroSource,
                    $"Macro '{name}' defined in {filepath}",
                    macroErrors
                );
                List<Token> scanResult =
                [
                    .. macroScanner.ScanTokens().Where(t => t.Type != TokenType.EOF),
                ];
                if (macroErrors.HasErrors)
                {
                    errors.Error("Defined macro couldn't be tokenised", filepath, _line, _column);
                }
                else
                {
                    _macros[name] = scanResult;
                }
                _line++;
                _column = 1;
            }
            else if (text == "#include")
            {
                while (char.IsWhiteSpace(peek()))
                {
                    advance();
                }
                _start = _current;
                string? path = filename();
                if (path is null)
                {
                    return;
                }
                string includeSource = "";
                try
                {
                    path = FileLookup.GetPath(path);
                    includeSource = File.ReadAllText(Path.GetFullPath(path));
                }
                catch (FileNotFoundException)
                {
                    errors.Error(
                        "The requested module couldn't be found in any of the library directories",
                        filepath,
                        _line,
                        _column - (_current - _start),
                        _current - _start
                    );
                }
                catch (DirectoryNotFoundException)
                {
                    errors.Error(
                        "Part of the path could not be found",
                        filepath,
                        _line,
                        _column - (_current - _start),
                        _current - _start
                    );
                }
                catch (ArgumentException)
                {
                    errors.Error(
                        "Invalid filepath",
                        filepath,
                        _line,
                        _column - (_current - _start),
                        _current - _start
                    );
                }

                if (_includeOnceAlreadyIncluded.Contains(path))
                {
                    return;
                }

                if (includedFrom?.Contains(path) ?? false)
                {
                    errors.Error(
                        "Circular include",
                        filepath,
                        _line,
                        _column - (_current - _start),
                        _current - _start
                    );
                    return;
                }

                Errors includeErrors = new();

                includeErrors.FilenameToSource[path] = includeSource;
                errors.FilenameToSource[path] = includeSource;

                HashSet<string> innerIncludedFrom = [filepath];
                foreach (string includedFromPath in includedFrom ?? [])
                {
                    innerIncludedFrom.Add(includedFromPath);
                }
                Scanner includeScanner = new(
                    includeSource,
                    path,
                    includeErrors,
                    innerIncludedFrom,
                    _includeOnceAlreadyIncluded,
                    _macros
                );
                (List<Token> tokens, Dictionary<string, List<Token>> macros) =
                    includeScanner.ScanTokensInIncludedFile();
                errors.Items.AddRange(includeErrors.Items);
                if (tokens.Any(t => t.Type == TokenType.IncludeOnce))
                {
                    _includeOnceAlreadyIncluded.Add(path);
                }
                _tokens.AddRange(tokens.Where(t => t.Type != TokenType.IncludeOnce));
                // also define macros from included file
                foreach (KeyValuePair<string, List<Token>> kvp in macros)
                {
                    _macros[kvp.Key] = kvp.Value;
                }
                foreach (var kvp in includeErrors.FilenameToSource)
                {
                    errors.FilenameToSource[kvp.Key] = kvp.Value;
                }
            }
            else if (text == "#once")
            {
                addToken(TokenType.IncludeOnce);
            }
            else if (_macros?.TryGetValue(text, out var replacement) ?? false)
            {
                foreach (Token t in replacement)
                {
                    addToken(t.Type, t.Literal);
                }
            }
            else if (_keywords.TryGetValue(text, out TokenType type))
            {
                addToken(type);
            }
            else
            {
                addToken(TokenType.Identifier, text);
            }
        }

        private string? filename()
        {
            while (alphaNumeric(peek()) || _allowedFilenameChars.Contains(peek()))
            {
                advance();
            }
            if (_current == _start)
            {
                errors.Error("Expected filename", filepath, _line, _column);
                return null;
            }
            else
            {
                return currentLexeme();
            }
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

        private string currentLexeme()
        {
            return _source[_start.._current];
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

        private void addToken(TokenType type, object? literal = null)
        {
            _tokens.Add(
                new(type, currentLexeme(), _line, _column - (_current - _start), filepath)
                {
                    Literal = literal,
                }
            );
        }

        private bool endReached()
        {
            return _current >= _source.Length;
        }
    }
}
