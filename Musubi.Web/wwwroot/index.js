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

function getStatusText(code) {
    return {
        100: "Continue",
        101: "Switching Protocols",
        102: "Processing",
        200: "OK",
        201: "Created",
        202: "Accepted",
        203: "Non-authoritative Information",
        204: "No Content",
        205: "Reset Content",
        206: "Partial Content",
        207: "Multi-Status",
        208: "Already Reported",
        226: "IM Used",
        300: "Multiple Choices",
        301: "Moved Permanently",
        302: "Found",
        303: "See Other",
        304: "Not Modified",
        305: "Use Proxy",
        307: "Temporary Redirect",
        308: "Permanent Redirect",
        400: "Bad Request",
        401: "Unauthorized",
        402: "Payment Required",
        403: "Forbidden",
        404: "Not Found",
        405: "Method Not Allowed",
        406: "Not Acceptable",
        407: "Proxy Authentication Required",
        408: "Request Timeout",
        409: "Conflict",
        410: "Gone",
        411: "Length Required",
        412: "Precondition Failed",
        413: "Payload Too Large",
        414: "Request-URI Too Long",
        415: "Unsupported Media Type",
        416: "Requested Range Not Satisfiable",
        417: "Expectation Failed",
        418: "I'm a teapot",
        421: "Misdirected Request",
        422: "Unprocessable Entity",
        423: "Locked",
        424: "Failed Dependency",
        426: "Upgrade Required",
        428: "Precondition Required",
        429: "Too Many Requests",
        431: "Request Header Fields Too Large",
        444: "Connection Closed Without Response",
        451: "Unavailable For Legal Reasons",
        499: "Client Closed Request",
        500: "Internal Server Error",
        501: "Not Implemented",
        502: "Bad Gateway",
        503: "Service Unavailable",
        504: "Gateway Timeout",
        505: "HTTP Version Not Supported",
        506: "Variant Also Negotiates",
        507: "Insufficient Storage",
        508: "Loop Detected",
        510: "Not Extended",
        511: "Network Authentication Required",
        599: "Network Connect Timeout Error"
    }[code] || ''
}

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
    code: response.status,
  };
}

async function submitHandler() {
  const source = sourceBox.value;
  const result = await runMusubi(source);
  statusP.textContent = result.code + " " + getStatusText(result.code);
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
source.value = exampleS.value;
