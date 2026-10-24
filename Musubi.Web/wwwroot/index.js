const API_BASE =
  window.location.hostname === "localhost" ||
  window.location.hostname === "127.0.0.1"
    ? "http://localhost:5103"
    : "https://musubi.schlafhase.uk";

const submit = document.querySelector("#submit");
const sourceBox = document.querySelector("#source");
const statusP = document.querySelector("#status");
const resultP = document.querySelector("#result");
const toggleTheme = document.querySelector("#theme-toggle");
const exampleS = document.querySelector("#example");

document.documentElement.dataset.theme = window.matchMedia(
  "(prefers-color-scheme: dark)",
)
  ? "dark"
  : "light";

async function runMusubi(source) {
  const response = await fetch(`${API_BASE}/run`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ source: source }),
  });

  const result =
    response.status == 429
      ? "Rate limited. Try again later."
      : await response.json();

  console.log(result);

  return {
    result: result,
    code: response.status + " " + response.statusText,
  };
}

async function submitHandler() {
  const source = sourceBox.value;
  const result = await runMusubi(source);
  statusP.textContent = result.code;
  resultP.textContent = result.result;
}

submit.addEventListener("click", submitHandler);
toggleTheme.addEventListener(
  "click",
  () =>
    (document.documentElement.dataset.theme =
      document.documentElement.dataset.theme == "dark" ? "light" : "dark"),
);

const examples = {
  "Hello World": `#include stdlib/useScottEncoding.mbim
#include stdlib/lists.mbim
"Hello World!"
`,
  "Fibonacci numbers": `#include stdlib
#include stdlib/lazyLists.mbim ; optional module in stdlib (not included by default)

let
  ; Lazy lists allow you to define infinite sequences.
  ; fibonacciNumbers is actually defined in lazyLists.mbim already 
  ; but I'll redefine it for this example.
  fibonacciNumbers :=
    let fibonacciNumbers' :=
      rec (\\fibonacciNumbers'.\\a -> \\b ->
        let
          element := a + b
        in
          ; Use \`lazy\` to delay evaluation of the tail (otherwise it would get stuck in an infinite loop)
          element : lazy fibonacciNumbers' b element
      )
    in
      fibonacciNumbers' 0 1,
in
  ; \`grab\` is defined in stdlib/lazyLists.mbim it takes n elements
  ; from a lazy list and returns them as a strict list
  grab 15 fibonacciNumbers | toString<List<ints>>
`,
  Factorial: `; You could just include stdlib. I'm just doing this
; to show which modules are actually needed.
#include stdlib/useScottEncoding.mbim
#include stdlib/recursion.mbim
#include stdlib/conversions.mbim
#include stdlib/pipe.mbim

let
  fac :=
    rec (\\fac.\\n. ; rec is the Z fix-point combinator
      n 1s (\\p.n * (fac p))
    )
in
  (fac 5s) | toString<ints>
`,
};

for (var propertyName in examples) {
  const example = examples[propertyName];
  var opt = document.createElement("option");
  opt.value = example;
  opt.innerHTML = propertyName;
  exampleS.appendChild(opt);
}

exampleS.addEventListener("change", (_) => (source.value = exampleS.value));
source.value = exampleS.value
