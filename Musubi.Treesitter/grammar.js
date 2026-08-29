module.exports = grammar({
  name: "musubi",

  extras: ($) => [/\s/, $.comment, $.macro, $.once, $.include],

  rules: {
    document: ($) => field("expression", $.expression),
    expression: ($) => prec.right(seq($._atom, repeat($._atom))),
    _atom: ($) =>
      choice(
        $.lambda,
        $.identifier,
        $.stringLiteral,
        $.charLiteral,
        $.numberLiteral,
        $.letin,
        $.paren,
      ),

    paren: ($) => seq("(", $.expression, ")"),

    letin: ($) =>
      seq(
        "let",
        seq($.definition, repeat(seq(",", $.definition))),
        optional(","),
        "in",
        $.expression,
      ),

    lambda: ($) =>
      seq(choice("\\", "λ"), $.identifier, choice(".", "->"), $.expression),

    definition: ($) =>
      seq(
        optional(seq($.infixKeyword, $.nonEncodedNumberLiteral)),
        $.identifier,
        ":=",
        $.expression,
      ),

    infixKeyword: (_) => choice("infix", "infixl", "infixr"),

    stringLiteral: (_) => /"(\\.|[^"\\\n])*"/,
    charLiteral: (_) => /'(\\[\\nt']|.)'/,
    numberLiteral: (_) => /\d+[csb]?/,
    identifier: (_) => /[^\sλ\(\)\.\\;,'"0-9][^\sλ\(\)\.\\;,]*/,
    nonEncodedNumberLiteral: (_) => /\d+/,

    macro: ($) => seq("#define", $.identifier, /[^\n]*/),
    once: (_) => "#once",
    comment: (_) => /;[^\n]*/,
    include: (_) => seq("#include", /[A-Za-z0-9_.\-\/~]+/),
  },
});
