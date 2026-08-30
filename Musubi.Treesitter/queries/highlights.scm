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
; (macro) @comment
; (once) @comment
; (include) @comment

; strings
(stringLiteral) @string
(escapeSequence) @string.escape

; delimiters
"," @punctuation.delimiter

; brackets
[
  "("
  ")"
] @punctuation.bracket

(identifier) @variable

; numbers
[
  (numberLiteral)
  (nonEncodedNumberLiteral)
] @number

