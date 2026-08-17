using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing
{
    public class Parser(List<Token> tokens, Errors errors)
    {
        private int _current;
        private readonly Stack<string> _knownVariables = [];
        private readonly Stack<string> _definedAliases = [];

        public Node Parse()
        {
            Node root = document();
            if (!endReached())
            {
                errors.ReportUnexpected(peek(), "end of file");
            }
            return root;
        }

        public Dictionary<string, Node> ParseModule()
        {
            return module();
        }

        // document: expression expressionEndSym?
        private Document document()
        {
            Node toplevel = expression();
            expect("a semicolon", TokenType.StatementEnd);
            return new() { Expression = toplevel };
        }

        // module: definition*
        private Dictionary<string, Node> module()
        {
            Dictionary<string, Node> mod = [];
            while (checkNext(TokenType.Identifier, TokenType.Definition))
            {
                (string name, Node value) = definition();
                mod.Add(name, value);
            }
            expect("end of file", TokenType.EOF);
            return mod;
        }

        // definition: identifier definitionSym expression expressionEndSym
        private (string name, Node value) definition()
        {
            string name = identifier();
            if (_definedAliases.Contains(name))
            {
                errors.ReportToken(previous(), $"Duplicate definition of alias '{name}'");
            }
            expect(TokenType.Definition);
            Node value = expression();
            expect("a semicolon", TokenType.StatementEnd);
            return (name, value);
        }

        // expression: atom (syntaxNoop | atom)*
        private Node expression()
        {
            Node left = atom();

            while (
                check(TokenType.Lambda, TokenType.Identifier, TokenType.LeftParen, TokenType.Number)
            )
            {
                Node right = atom();
                left = new Application() { Function = left, Argument = right };
            }

            return left;
        }

        // atom: lambda | variable | "(" expression ")" | number | letin | includein
        private Node atom()
        {
            switch (peek().Type)
            {
                case TokenType.Lambda:
                    return lambda();
                case TokenType.Identifier:
                    string id = identifier();
                    // find 0-based De Bruijn index

                    int debruijn = 0;
                    foreach (string name in _knownVariables)
                    {
                        if (name == id)
                        {
                            return new Variable() { DeBruijn = debruijn };
                        }
                        debruijn++;
                    }
                    if (_definedAliases.Contains(id))
                    {
                        return new DefinitionReference() { Definition = id };
                    }
                    errors.ReportToken(previous(), $"Undefined identifier '{id}'");
                    return new Error();
                case TokenType.LeftParen:
                    advance();
                    Node inner = expression();
                    expect("closing parenthesis", TokenType.RightParen);
                    return inner;
                case TokenType.Number:
                    advance();
                    return new Number() { Value = (int)previous().Literal! };
                case TokenType.Let:
                    return letIn();
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
                    _knownVariables.Push(captured);
                    Node body = expression();
                    _knownVariables.Pop();
                    return new Lambda() { Body = body };
                }
            }
            return new Error();
        }

        // letIn: "let" definition* "in" expression
        private Node letIn()
        {
            Dictionary<string, Node> definitions = [];
            if (expect(TokenType.Let))
            {
                while (!check(TokenType.In))
                {
                    (string? name, Node? value) = definition();
                    if (name is not null)
                    {
                        definitions[name] = value;
                    _definedAliases.Push(name);
                    }
                }
                expect(TokenType.In);
                Node body = expression();
                foreach (var _ in definitions)
                {
                    _definedAliases.Pop();
                }
                return new LetIn() { Definitions = definitions, Expression = body };
            }
            return new Error();
        }

        private string identifier()
        {
            expect(TokenType.Identifier);
            return (previous().Literal as string)!;
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
                return types.Contains(TokenType.EOF);
            }
            return types.Contains(peek().Type);
        }

        private bool checkNext(params TokenType[] typeSequence)
        {
            if (_current + typeSequence.Length >= tokens.Count)
            {
                return false;
            }
            for (int i = 0; i < typeSequence.Length; i++)
            {
                if (typeSequence[i] != tokens[_current + i].Type)
                {
                    return false;
                }
            }
            return true;
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
