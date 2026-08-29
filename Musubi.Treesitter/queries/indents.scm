(expression) @indent.begin

"(" @indent.begin
")" @indent.end

(lambda) @indent.begin

(letin
  "let" @indent.begin
  "in" @indent.end)

(letin
  "in" @indent.begin) @indent.end
