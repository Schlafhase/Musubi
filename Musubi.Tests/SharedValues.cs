using Musubi.Compiler.Scanning;

namespace Musubi.Tests
{
    public static class SharedValues
    {
        public const string ModuleSource = """
            let
                a := 123,
                b := "hello",
                c := 'c',
                d := someId,
                ns := 10s,
                nc := 15c,
                nb := 17b,
            in
            """;

        public static TokenType[] ModuleExpectedTokens =
        [
            TokenType.Let,
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Number,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.String,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Char,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.Identifier,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.ScottNumber,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.ChurchNumber,
            TokenType.ListSeparator,
            //
            TokenType.Identifier,
            TokenType.Definition,
            TokenType.BinaryNumber,
            TokenType.ListSeparator,
            //
            TokenType.In,
            TokenType.EOF,
        ];

        public const string DirectivesSource = """
            #once
            #include basic.mbi
            #define -> "helo"
            c
            ->
            #once
            """;

        public const string NestedIncludeSource = """
            #include nested.mbi
            world
            """;
    }
}