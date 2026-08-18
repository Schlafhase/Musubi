using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing
{
    public class Parser(List<Token> tokens, Errors errors)
    {
        private int _current;

        // keeps track of variables (captured by labdas) in the current context
        private readonly Stack<string> _knownVariables = [];

        // keeps track of aliases (using :=) in the current context
        private readonly Stack<(string name, CodeRange definedAt)> _definedAliases = [];

        public Node Parse()
        {
            Node root = document();
            if (!endReached())
            {
                errors.UnexpectedError(peek(), "end of file");
            }
            return root;
        }

        // document: expression expressionEndSym?
        private Document document()
        {
            Node toplevel = expression();
            return new() { Expression = toplevel };
        }

        // definition: identifier definitionSym expression expressionEndSym
        private (string name, CodeRange definedAt, Node value) definition()
        {
            string name = identifier();
            Token idToken = previous();
            (string name, CodeRange definedAt)[] previousDefinitions =
            [
                .. _definedAliases.Where(a => a.name == name),
            ];
            if (previousDefinitions.Length > 0)
            {
                errors.TokenHint(
                    previous(),
                    $"Redefinition of '{name}' hides the previous definition of '{name}' at {previousDefinitions[0].definedAt.Location}"
                );
            }
            expect(TokenType.Definition);
            Node value = expression();
            return (name, idToken.Range, value);
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
                    if (_definedAliases.Any(a => a.name == id))
                    {
                        return new DefinitionReference() { Definition = id };
                    }
                    errors.TokenError(previous(), $"Undefined identifier '{id}'");
                    return new SyntaxError();
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
                    errors.UnexpectedError(peek(), "expression");
                    return new SyntaxError();
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
            return new SyntaxError();
        }

        // letIn: "let" definition* "in" expression
        private Node letIn()
        {
            Dictionary<string, Node> definitions = [];
            if (expect(TokenType.Let))
            {
                if (match(TokenType.In))
                {
                    errors.TokenError(previous(), "Empty let-in expression");
                    return new SyntaxError();
                }
                for (; ; )
                {
                    (string? name, CodeRange? definedAt, Node? value) = definition();
                    if (name is null)
                    {
                        return new SyntaxError();
                    }
                    definitions[name] = value;
                    _definedAliases.Push((name, definedAt));
                    if (!match(TokenType.ListSeparator))
                    {
                        break;
                    }
                }
                expect("'in' or a comma", TokenType.In);
                Node body = expression();
                foreach (var _ in definitions)
                {
                    _definedAliases.Pop();
                }
                return new LetIn() { Definitions = definitions, Expression = body };
            }
            return new SyntaxError();
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
            errors.UnexpectedError(next, types);
            return false;
        }

        private bool expect(string expected, params TokenType[] types)
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.UnexpectedError(next, expected);
            return false;
        }

        private bool endReached()
        {
            return peek().Type == TokenType.EOF;
        }
    }
}
