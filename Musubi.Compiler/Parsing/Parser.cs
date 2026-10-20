using System.Collections.Immutable;
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
            public string? DocString;
            public Node Value;
            public CodeRange DefinedAt;
            public Token IdToken;
            public CodeLocation End;
            public bool Infix;
            public int BindingPrecedence;
            public Associativity Associativity;
        }

        private int _current;

        // keeps track of variables (captured by labdas) in the current context
        private readonly Stack<string> _knownVariables = [];

        // keeps track of aliases (using :=) in the current context
        private readonly Stack<Definition> _definedAliases = [];

        private readonly List<Token> _hoverTokens = [];
        private readonly List<(CodeRange range, Token symbol)> _contexts = [];

        public readonly struct ParseResult
        {
            public readonly Node Root { get; init; }
            public readonly IReadOnlyList<Token> HoverTokens { get; init; }
            public readonly IReadOnlyList<(CodeRange range, Token symbol)> Contexts { get; init; }
        }

        public ParseResult Parse()
        {
            Node root = document();
            if (!endReached())
            {
                errors.UnexpectedError(peek(), "end of file");
            }
            return new ParseResult()
            {
                Root = root,
                HoverTokens = _hoverTokens,
                Contexts = _contexts,
            };
        }

        // document: expression expressionEndSym?
        private Document document()
        {
            Node toplevel = expression();
            return new() { Expression = toplevel };
        }

        // definition: identifier definitionSym expression expressionEndSym
        private Definition? definition()
        {
            string? docString = peek().DocString;
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
                && expectAndSkipToSafeToken(
                    "a number defining the binding precedence of this operator",
                    [TokenType.ListSeparator, TokenType.In],
                    TokenType.Number
                )
            )
            {
                infix = true;
                bindingPrecedence = (int)previous().Literal!;
            }

            string? name = identifier([
                TokenType.Definition,
                TokenType.In,
                TokenType.ListSeparator,
            ]);
            Token idToken = previous();
            Token hoverToken = new(
                idToken.Type,
                idToken.Lexeme,
                idToken.Line,
                idToken.Column,
                idToken.Filename
            )
            {
                DocString = docString,
            };
            _hoverTokens.Add(hoverToken);
            Definition[] previousDefinitions = [.. _definedAliases.Where(a => a.Name == name)];
            if (previousDefinitions.Length > 0)
            {
                errors.TokenHint(
                    previous(),
                    $"Redefinition of '{name}' hides the previous definition of '{name}' at {previousDefinitions[0].DefinedAt.A}"
                );
            }

            if (name is not null)
            {
                expect("':='", TokenType.Definition);
            }
            else
            {
                // if identifier failed to parse don't emit error for missing :=
                match(TokenType.Definition);
            }

            if (peek().Type == TokenType.Identifier && (string)peek().Literal! == "rec")
            {
                int offset = 1;
                for (; peek(offset).Type == TokenType.LeftParen; offset++)
                    ;
                offset++; // skip the lambda
                if (peek(offset).Type == TokenType.Identifier)
                {
                    string idName = (string)peek(offset).Literal!;
                    if (idName != name)
                    {
                        errors.TokenWarning(
                            peek(offset),
                            $"When using the fix-point combinator ('rec'), a lambda should name its first argument exactly like the definition that defines it to make the recursion clear. This argument should be named {name}"
                        );
                    }
                }
            }
            Node value = expression();
            value.DocString = docString;
            return new Definition()
            {
                Name = name ?? "",
                DocString = docString,
                DefinedAt = idToken.Range,
                IdToken = idToken,
                End = idToken?.IncludedAt?.B ?? previous().Range.B,
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
                Definition? def_ = null;
                try
                {
                    def_ = _definedAliases.First(a => a.Name == (string)op.Literal!);
                    Definition def = def_.Value;
                    string signature =
                        (
                            def.Infix
                                ? (
                                    def.Associativity switch
                                    {
                                        Associativity.Left => "infixl",
                                        Associativity.Right => "infixr",
                                        _ => "infix",
                                    }
                                )
                                    + " "
                                    + def.BindingPrecedence
                                    + " "
                                : ""
                        ) + def.Name;
                    Token hoverToken = op with
                    {
                        DocString = "`" + signature + "`\n" + def.DocString,
                    };
                    _hoverTokens.Add(hoverToken);
                }
                catch { }

                if (op.Type is TokenType.Identifier && def_.HasValue && def_.Value.Infix)
                {
                    Definition def = def_.Value;
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

                    int debruijn = 0;
                    foreach (string name in _knownVariables)
                    {
                        if (name == def.Name)
                        {
                            break;
                        }
                        debruijn++;
                    }

                    left = new Application()
                    {
                        Function = new Application()
                        {
                            Function = new Variable()
                            {
                                ReferencedVariable = def.Name,
                                ReferencedDefinition = def.Value,
                                DeBruijn = debruijn,
                            },
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
                    string id = identifier()!;
                    // find 0-based De Bruijn index

                    int debruijn = 0;
                    foreach (string name in _knownVariables)
                    {
                        if (name == id)
                        {
                            Node? referencedDefinition = null;
                            try
                            {
                                Definition def = _definedAliases.First(a => a.Name == id);
                                string signature =
                                    (
                                        def.Infix
                                            ? (
                                                def.Associativity switch
                                                {
                                                    Associativity.Left => "infixl",
                                                    Associativity.Right => "infixr",
                                                    _ => "infix",
                                                }
                                            )
                                                + " "
                                                + def.BindingPrecedence
                                                + " "
                                            : ""
                                    ) + def.Name;
                                Token hoverToken = previous() with
                                {
                                    DocString = "`" + signature + "`\n" + def.DocString,
                                };
                                referencedDefinition = def.Value;
                                _hoverTokens.Add(hoverToken);
                            }
                            catch (InvalidOperationException) { }
                            return new Variable()
                            {
                                DeBruijn = debruijn,
                                ReferencedVariable = name,
                                ReferencedDefinition = referencedDefinition,
                            };
                        }
                        debruijn++;
                    }
                    // if (_definedAliases.Any(a => a.Name == id))
                    // {
                    //     return new DefinitionReference() { Definition = id };
                    // }
                    errors.TokenError(previous(), $"Undefined identifier '{id}'");
                    return new SyntaxError();
                case TokenType.LeftParen:
                    advance();
                    Node inner = expression();
                    expect("closing parenthesis", TokenType.RightParen);
                    return inner;
                case TokenType.Number:
                    advance();
                    return new Number()
                    {
                        Value = (int)previous().Literal!,
                        Encoding = NumeralEncoding.Scott,
                    };
                case TokenType.BinaryNumber:
                    advance();
                    return new Number()
                    {
                        Value = (int)previous().Literal!,
                        Encoding = NumeralEncoding.Binary,
                    };
                case TokenType.ScottNumber:
                    advance();
                    return new Number()
                    {
                        Value = (int)previous().Literal!,
                        Encoding = NumeralEncoding.Scott,
                    };
                case TokenType.ChurchNumber:
                    advance();
                    return new Number()
                    {
                        Value = (int)previous().Literal!,
                        Encoding = NumeralEncoding.Church,
                    };
                case TokenType.Let:
                    return letIn();
                default:
                    errors.UnexpectedError(peek(), "expression");
                    advance();
                    return new SyntaxError();
            }
        }

        // lambda: ("\" | "λ") identifier "." expression
        private Node lambda()
        {
            if (expect(TokenType.Lambda))
            {
                string captured = identifier() ?? "";
                if (expect(TokenType.Dot))
                {
                    _knownVariables.Push(captured);
                    Node body = expression();
                    _knownVariables.Pop();
                    return new Lambda()
                    {
                        Body = body,
                        CapturedVariable = captured,
                        DebugIdentifier = $"\\{captured}",
                    };
                }
            }
            return new SyntaxError();
        }

        // letIn: "let" definition* "in" expression
        private Node letIn()
        {
            // TODO: definitions that do not reference outer variables
            // can be optimised by creating a global C function for them
            // instead of creating an extra lambda.
            List<Definition> definitions = [];
            if (expect(TokenType.Let))
            {
                CodeLocation start = previous().Range.A;
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
                    Definition? def = definition();
                    if (def is null)
                    {
                        continue;
                    }
                    Definition defNotNull = def.Value;
                    // NOTE: This skips unused definition but this is far from perfect since this
                    // 1. doesn't respect context
                    // 2. only cares about the name of the identifier, not what it actually refers to
                    // 3. still adds the definition if another definition (which can be unused) depends on it
                    // could definitely be improved in the future but it's better than nothing
                    if (
                        !tokens[_current..]
                            .Any(t =>
                                t.Type == TokenType.Identifier
                                && (string)t.Literal! == defNotNull.Name
                            )
                    )
                    {
                        goto next;
                    }
                    if (defNotNull.Name is null)
                    {
                        return new SyntaxError();
                    }
                    definitions.Add(defNotNull);
                    _knownVariables.Push(defNotNull.Name);
                    _definedAliases.Push(defNotNull);

                    next:
                    if (!match(TokenType.ListSeparator))
                    {
                        break;
                    }
                }
                expect("'in' or a comma", TokenType.In);
                Node body = expression();
                Application? bodyWrapper = null;
                foreach (Definition def in definitions)
                {
                    Token next =
                        _current + 1 < tokens.Count
                            ? tokens[_current + 1]
                            : new Token(0, "", int.MaxValue, 0, "");
                    if (next.Filename == def.End.Filename)
                    {
                        _contexts.Add(
                            (
                                new(def.End, next.Range.A),
                                new Token(TokenType.Identifier, def.Name, 0, 0, string.Empty) with
                                {
                                    DocString = def.DocString,
                                }
                            )
                        );
                    }
                }
                foreach (Definition def in definitions.Reverse<Definition>())
                {
                    _knownVariables.Pop();
                    _definedAliases.Pop();
                    // a definition let x := y, z:=w in x should be sugar for
                    // (\x.(\z.x) y) w
                    bodyWrapper = new Application()
                    {
                        Function = new Lambda()
                        {
                            Body = bodyWrapper ?? body,
                            CapturedVariable = def.Name,
                            DebugIdentifier = $"Desugared from let in definition for {def.Name}",
                        },
                        Argument = def.Value,
                    };
                }
                return bodyWrapper ?? body;
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

        private string? identifier(ICollection<TokenType>? nextSafe = null)
        {
            if (expectAndSkipToSafeToken("identifier", nextSafe, TokenType.Identifier))
            {
                return previous().Literal as string;
            }
            return null;
        }

        // helpers

        private void advance()
        {
            if (!endReached())
            {
                _current++;
            }
        }

        private Token previous()
        {
            return tokens[_current - 1];
        }

        private Token peek(int offset = 0)
        {
            if (_current + offset >= tokens.Count)
            {
                return new Token(
                    TokenType.EOF,
                    "",
                    previous().Line,
                    previous().Column,
                    previous().Filename
                );
            }
            return tokens[_current + offset];
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
            return expectAndSkipToSafeToken(expected, null, types);
        }

        private bool expectAndSkipToSafeToken(
            string expected,
            ICollection<TokenType>? nextSafe = null,
            params TokenType[] types
        )
        {
            if (match(types))
            {
                return true;
            }
            Token next = peek();
            errors.UnexpectedError(next, expected);
            if (nextSafe is not null)
            {
                while (!nextSafe.Contains(peek().Type) && !endReached())
                {
                    advance();
                }
            }
            advance();
            return false;
        }

        private bool endReached()
        {
            return peek().Type == TokenType.EOF;
        }
    }
}
