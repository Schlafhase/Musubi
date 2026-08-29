; keywords
[
  "let"
  "in"
  "infix"
  "infixl"
  "infixr"
  "\\"
  "λ"
  "."
  "->"
  ":="
] @keyword

; comments
(comment) @comment
(macro) @comment
(once) @comment
(include) @comment

; strings
(stringLiteral) @string

; delimiters
"," @punctuation.delimiter

; brackets
[
  "("
  ")"
] @punctuation.bracket

(identifier) @variable

