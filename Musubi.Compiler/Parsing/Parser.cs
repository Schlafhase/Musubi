using Musubi.Compiler.Nodes;
using Musubi.Compiler.Scanning;

namespace Musubi.Compiler.Parsing
{
    public class Parser(List<Token> tokens, Errors errors)
    {
        private enum Associativity
        {
            Right,
            Left,
            None,
        }

        private struct Definition
        {
            public string Name;
            public Node Value;
            public CodeRange DefinedAt;
            public bool Infix;
            public int BindingPrecedence;
            public Associativity Associativity;
        }

        private int _current;

        // keeps track of variables (captured by labdas) in the current context
        private readonly Stack<string> _knownVariables = [];

        // keeps track of aliases (using :=) in the current context
        private readonly Stack<Definition> _definedAliases = [];

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
        private Definition definition()
        {
            int bindingPrecedence = int.MaxValue;
            bool infix = false;
            Associativity associativity = Associativity.Left;
            if (check(TokenType.Infix))
            {
                associativity = Associativity.None;
            }
            else if (check(TokenType.Infixl))
            {
                associativity = Associativity.Left;
            }
            else if (check(TokenType.Infixr))
            {
                associativity = Associativity.Right;
            }
            if (
                match(TokenType.Infix, TokenType.Infixl, TokenType.Infixr)
                && expect(
                    "a number defining the binding precedence of this operator",
                    TokenType.Number
                )
            )
            {
                infix = true;
                bindingPrecedence = (int)previous().Literal!;
            }

            string name = identifier();
            Token idToken = previous();
            if (name.Length >= 3 && name[0] == '\'' && name[2] == '\'')
            {
                errors.TokenWarning(
                    idToken,
                    "Identifier looks like a character literal. If this is intentional, consider renaming it because it makes code unreadable."
                );
            }
            Definition[] previousDefinitions = [.. _definedAliases.Where(a => a.Name == name)];
            if (previousDefinitions.Length > 0)
            {
                errors.TokenHint(
                    previous(),
                    $"Redefinition of '{name}' hides the previous definition of '{name}' at {previousDefinitions[0].DefinedAt.Location}"
                );
            }
            expect(TokenType.Definition);
            Node value = expression();
            return new Definition()
            {
                Name = name,
                DefinedAt = idToken.Range,
                Value = value,
                Infix = infix,
                BindingPrecedence = bindingPrecedence,
                Associativity = associativity,
            };
        }

        // expression: atom (syntaxNoop | atom)*
        private Node expression(int minBindingPrecedence = 0)
        {
            Node left = atom();

            while (
                check(
                    TokenType.Lambda,
                    TokenType.Identifier,
                    TokenType.LeftParen,
                    TokenType.Number,
                    TokenType.ScottNumber,
                    TokenType.ChurchNumber,
                    TokenType.BinaryNumber
                )
            )
            {
                // check for infix operators
                Token op = peek();
                if (
                    op.Type is TokenType.Identifier
                    && _definedAliases.Any(a => a.Name == (string)op.Literal! && a.Infix)
                )
                {
                    Definition def = _definedAliases.First(a =>
                        a.Name == (string)op.Literal!
                    );
                    if (def.BindingPrecedence < minBindingPrecedence)
                    {
                        break;
                    }

                    int nextMinBindingPrecedence = def.Associativity switch
                    {
                        Associativity.Left => def.BindingPrecedence + 1,
                        Associativity.Right => def.BindingPrecedence,
                        Associativity.None => def.BindingPrecedence + 1,
                        _ => throw new NotImplementedException(),
                    };

                    advance();
                    Node rhs = expression(nextMinBindingPrecedence);

                    if (def.Associativity == Associativity.None)
                    {
                        if (
                            peek().Type == TokenType.Identifier
                            && _definedAliases.Any(a =>
                                a.Name == (string)peek().Literal! && a.Infix
                            )
                        )
                        {
                            Definition nextDef = _definedAliases.First(a =>
                                a.Name == (string)peek().Literal!
                            );
                            if (
                                nextDef.BindingPrecedence == def.BindingPrecedence
                                && nextDef.Associativity == Associativity.None
                            )
                            {
                                errors.TokenError(
                                    peek(),
                                    "Chaining non-associative oparators is not allowed. Please use parentheses in this case"
                                );
                                return new SyntaxError();
                            }
                        }
                    }

                    left = new Application()
                    {
                        Function = new Application()
                        {
                            Function = new DefinitionReference() { Definition = def.Name },
                            Argument = left,
                        },
                        Argument = rhs,
                    };
                    continue;
                }

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
                    if (_definedAliases.Any(a => a.Name == id))
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
                case TokenType.ScottNumber:
                    advance();
                    return new Number() { Value = (int)previous().Literal! };
                case TokenType.ChurchNumber:
                    advance();
                    return new Number() { Value = (int)previous().Literal!, ChurchEncoded = true };
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
                    return new Lambda() { Body = body, DebugIdentifier = $"\\{captured}" };
                }
            }
            return new SyntaxError();
        }

        // letIn: "let" definition* "in" expression
        private Node letIn()
        {
            List<Definition> definitions = [];
            Dictionary<string, Node> optimisedDefinitions = [];
            if (expect(TokenType.Let))
            {
                if (match(TokenType.In))
                {
                    errors.TokenError(previous(), "Empty let-in expression");
                    return new SyntaxError();
                }
                for (; ; )
                {
                    if (check(TokenType.In)) // Allow optional comma on last definition
                    {
                        break;
                    }
                    Definition def = definition();
                    if (def.Name is null)
                    {
                        return new SyntaxError();
                    }
                    if (def.Name == "+")
                    {
                        Console.Write("");
                    }
                    if (usesOuterVariables(def.Value))
                    {
                        definitions.Add(def);
                        _knownVariables.Push(def.Name);
                    }
                    else
                    {
                        // doesn't use outer variables, can be optimised by the compiler
                        optimisedDefinitions.Add(def.Name, def.Value);
                        _definedAliases.Push(def);
                    }
                    if (!match(TokenType.ListSeparator))
                    {
                        break;
                    }
                }
                expect("'in' or a comma", TokenType.In);
                Node body = expression();
                Node? bodyWrapper = null;
                foreach (Definition def in definitions)
                {
                    _knownVariables.Pop();
                    // a definition let x := y in x should be sugar for
                    // (\x.x) y
                    bodyWrapper = new Application()
                    {
                        Function = new Lambda()
                        {
                            Body = bodyWrapper ?? body,
                            DebugIdentifier = $"Desugared from let in definition for {def.Name}",
                        },
                        Argument = def.Value,
                    };
                }
                foreach (var _ in optimisedDefinitions)
                {
                    _definedAliases.Pop();
                }
                return optimisedDefinitions.Count > 0
                    ? new LetIn()
                    {
                        Definitions = optimisedDefinitions,
                        Expression = bodyWrapper ?? body,
                    }
                    : bodyWrapper ?? body;
            }
            return new SyntaxError();
        }

        private bool usesOuterVariables(Node node, int depth = 0)
        {
            return node switch
            {
                Variable v => v.DeBruijn >= depth,
                Application a => usesOuterVariables(a.Function, depth)
                    || usesOuterVariables(a.Argument, depth),
                DefinitionReference => false,
                Document => false,
                SyntaxError => false,
                Lambda l => usesOuterVariables(l.Body, depth + 1),
                LetIn li => usesOuterVariables(li.Expression, depth),
                Number => false,
                _ => true, // default true because even if it doesn't use outer variables, the compiler can handle compiling this case
            };
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