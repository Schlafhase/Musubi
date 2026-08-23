# Musubi: Raw functional programming 🍣

![time spent](https://hackatime.hackclub.com/api/v1/badge/U081Z7D7YJK/Schlafhase/Musubi)

Musubi is a functional programming language designed to be as close to the
Lambda Calculus as possible while still being usable.

## Table of contents

- [The compiler](#the-compiler)
  - [Installation](#installation)
    - [Using dotnet tools](#using-dotnet-tools)
    - [Manual installation](#manual-installation)
    - [Installation of the standard library](#installation-of-the-standard-library)
  - [Usage](#usage)
    - [Compile](#compile)
    - [Run](#run)
- [Introduction](#introduction)
  - [Lambda Calculus basics](#lambda-calculus-basics)
  - [Computation in the Lambda Calculus](#computation-in-the-lambda-calculus)
- [Output](#output)
- [Musubi's Syntax](#musubis-syntax)
  - [Identifiers](#identifiers)
  - [Features](#features)
    - [Let-in expression](#let-in-expression)
    - [Comments](#comments)
    - [Infix operators](#infix-operators)
    - [Including files](#including-files)
    - [The #once directive](#the-once-directive)
    - [Literals](#literals)
      - [Numerals](#numerals)
      - [Characters](#characters)
      - [Strings](#strings)
    - [The #define directive](#the-define-directive)
- [Writing Modules](#writing-modules)
- [The rest](#the-rest)

## The compiler

### Installation

<!--Yes, this section is mostly copied from Waddle :D-->

#### Using dotnet tools

If you have the .NET SDK installed, you can install Waddle with this simple
command:

```sh
dotnet tool install -g Musubi
```

#### Manual installation

> [!NOTE]
>
> The binary for Windows hasn't been tested yet

<!--TODO: test windows version 😔-->

Grab the binary from the
[latest release](https://github.com/Schlafhase/Musubi/releases/) (`musubi` for
Linux and `musubi.exe` for Windows). Move the binary to a directory in your
PATH. You should be ready to go now.

#### Installation of the standard library

It is highly recommended to install the standard library as well. Refer to
[MusubiStdlib](https://github.com/Schlafhase/MusubiStdlib) for installation
instructions.

### Usage

#### Compile

Use `musubi compile <file> [options]` to compile a file. When `<file>` is not
specified, `main.mbi` will be used.

| Option | Description                                             | Allowed Values                                         | Default                     |
| ------ | ------------------------------------------------------- | ------------------------------------------------------ | --------------------------- |
| `-l`   | The language to compile to                              | `C`, `LC`                                              | `C`                         |
| `-o`   | The name of the output file                             | string                                                 | `compiled.c`/`compiled.lam` |
| `-i`   | Additional directories to look in for `#include`d files | list of strings separated by spaces                    | empty                       |
| `-d`   | Print debug information (for developing Musubi)         | Boolean (just writing `-d` without a value means true) | `false`                     |

#### Run

Use `musubi run <file> [options]` to run a file. When `<file>` is not specified,
`main.mbi` will be used. This command has to run in a writeable directory.

| Option | Description                                             | Allowed Values                                         |
| ------ | ------------------------------------------------------- | ------------------------------------------------------ |
| `-i`   | Additional directories to look in for `#include`d files | list of strings separated by spaces                    |
| `-c`   | The C compiler to use                                   | string                                                 |
| `-d`   | Print debug information (for developing Musubi)         | Boolean (just writing `-d` without a value means true) |

## Introduction

Musubi is a superset of the Lambda Calculus. This means that every valid Lambda
Calculus program is also a valid Musubi program.

### Lambda Calculus basics

For those who don't know: Lambda Calculus (LC) is the most basic functional
programming language in which you only have anonymous functions, also referred
to as "Lambdas". Theres three types of syntax pieces in LC:

| Syntax     | Name        | Description                                                                                                                                                                | Python equivalent |
| ---------- | ----------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------- |
| `λx.{...}` | Lambda      | A function which takes exactly one `x` (can be any name) as its parameter and returns a body that can use x.                                                               | `lambda x: {...}` |
| `x y`      | Application | Calls a lambda with an argument.                                                                                                                                           | `x(y)`            |
| `x (y z)`  | Parentheses | Should be self-explanatory. Parentheses can be used to specify order of evaluation. Application in LC is **left associative** with the lowest possible binding precedence. | `x(y(z))`         |

The confusing part is that you really only have these lambdas. So we can
conclude that every lambda has to take a lambda as it's parameter (`x` in the
above example) and the return value must also be a lambda because that's all
there is in LC.

Lambdas only ever taking exactly one parameter may seem limiting but there is
actually a simple trick to have a function use more parameters. It's called
**currying** (after Haskell Brooks Curry) and can look like this:

> [!NOTE]
>
> Definitions like the one below are **not** valid LC but there is
> [syntax in Musubi for them](#musubis-syntax).

```
curriedLambda := \λa.\λb.a b
```

What this means is: A lambda that takes an `a` and returns another lambda that
takes a `b` and returns `a` applied to `b`. This means we can now provide both
arguments like this:

```
curriedLambda x y
```

`curriedLambda x` returns a lambda that expects one more parameter which is then
provided (`y`). This also gives us another useful feature: partial application.
Let's pretend, for this example, that the `+` operator is defined:

```
add := λa.λb.a + b
```

Now we can use `add 5` (more on how numbers work later) which would return a
function that takes a `b` and returns `5 + b`.

### Computation in the Lambda Calculus

So how can we achieve computation with just functions? It has actually been
proven that
[Turing computability and so called λ-computability are equivalent ](https://en.wikipedia.org/wiki/Church%E2%80%93Turing_thesis).
The key to programming in LC is using encodings for datatypes (and recursion
which is explained in the standard library). Since you only have lambdas, you
can try to model different datatypes as certain types of functions. For example,
let's define true and false:

```
true := λa.λb.a,
false := λa.λb.b
```

True is a function that takes two parameters and returns the first one while
false takes two parameters and returns the second one. Now we can do simple
branching:

```
someBoolean x y
```

If `someBoolean` is true, this evaluates to `x` otherwise it evaluates to `y`.
Similar techniques can be used to encode numbers, lists and many other
datatypes. Since these encodings aren't core features of Musubi or LC, I won't
go more in-depth on them here but check out the
[Musubi standard library](https://github.com/Schlafhase/MusubiStdlib) for more
details on datatype encodings and other utilities that allow programming.

## Output

The compiled C program expects a Musubi program to evaluate to a string (encoded
as a scott list of scott numerals). That string will be printed but if the
program evaluates to something unexpected (or a string that's too long) it will
likely crash with a segmentation fault.

## Musubi's Syntax

All the syntax of LC is also valid in Musubi (a backslash can be used in place
of a lambda) but Musubi introduces a bit of extra syntax to make it an actually
usable language but all syntax is just so called
[syntactic sugar](https://en.wikipedia.org/wiki/Syntactic_sugar) for equivalent
LC syntax.

### Identifiers

Musubi gives you a lot of freedom with identifier names. An identifier must:

1. not contain any of the following characters: ` `, `\t`, `\n`, `\r`, `(`, `)`,
   `.`, `\\`, `λ`, `;`, `,`, `\0`
2. not start with a digit
3. not start with `'{char}'` where {char} is any single character
4. not start with `"`
5. not start with `:=`
6. not be a keyword

Having any of these characters in your identifier won't necessarily cause an
error but it will not be parsed as an identifier (which may be unexpected if you
don't know these rules).

### Features

#### Let-in expression

The most notable feature is the `let ... in ...` expression. It allows giving
names to certain expressions to avoid rewriting them all over again. They follow
this pattern:

> [!NOTE]
>
> In this and all following examples, `identifierN` and `expressionN` can be
> **any** identifier or expression

```
let
  identifier1 := expression1,
  identifier2 := expression2,
  ...
  identifierN := expressionN
in
  expression0
```

Definitions are separated by a comma (a comma after the last definition is
optional). The let in expression is equivalent to:

```
(\identifier1.
  (\identifer2.
    ...
    (\identifierN.
      expression0
    ) expressionN
    ...
  ) expression2
) expression1
```

##### Example

```
let
  true := \a.\b.a,
  false := \a.\b.b
in
  ...
```

#### Comments

Another very useful feature that you only really notice when it's missing (like
in JSON) are comments. Musubi uses a semicolon `;` for comments. Any character
on a line after a semicolon will be ignored. The semicolon was chosen because
other common characters like '/', '-', '#' are very useful for operators so I
wanted to allow them in identifier names.

##### Example

```
... ; this is a comment
```

#### Infix operators

This feature, inspired by Haskell, allows you to tell the parser to parse a
named expression (from a let-in expression) as an infix operator. For this you
use this syntax:

```
let
  infix[l/r] {binding precedence} identifier1 := expression1,
  ...
in
  ...
```

This lets you write `a identifier1 b` instead of the usual `identifier1 a b`.
You can use `infixl`, `infixr` or `infix` to declare the operator as, **left
associative**, **right associative** or **not associative**. Left and right
should be pretty obvious. No associativity just means that chaining this
operator with another non-associative operator is not allowed (evaluation order
must be specified with parentheses).

##### Example

```
let
  ; assuming add and all the other undefined functions referenced here are defined
  infixl 100 + := add, ; Add is left associative with a relatively low binding precedence
  infixl 200 * := mult, ; Mult binds stronger than add
  infixr 50 : := cons, ; Cons is right associative so I can write `1 : 2 : 3 : nil`
  infix = := eq ; Eq should be non-associative since `a = b = c` does not evaluate to what one would expect
in
...
```

#### Including files

Similar to C, Musubi lets you include files with `#include {file}`. This is
equivalent to pasting the content of `{file}` in the same place where it was
`#included` **as long as the content of `{file}` tokenises correctly**. This
also means that **the order of includes can matter**.

`{file}` can be an absolute path to a file or a relative path. If it's relative,
the compiler will look in ~/.musubi for that file but additional directories can
be specified (see [usage of the compiler](#usage)).

`{file}` may also be a directory in which case `{file}/module.mbim` will be
included.

Circular includes are not allowed.

##### Example

```
; Assuming the stdlib is installed in ~/.musubi
#include stdlib ; stdlib is a directory so this includes stdlib/module.mbim
"Hello World!"
```

#### The #once diretive

Writing `#once` anywhere in your file tells the compiler to only include it once
in the same context. This is useful for module files which only provide
definitions because there's no point in redefining the definitions.

#### Literals

For convenience, Musubi provides number, character and string literals which
expand to a certain encoding for these datatypes. These literals make most sense
when used together with the
[standard library](https://github.com/Schlafhase/MusubiStdlib) which defines all
the used encodings and useful functions that operate on them.

##### Numerals

Integer literals expand to a scott encoded integer by default but the encoding
can be specified using one of 's', 'c' or 'b' as suffix which stand for "scott",
"church" and "binary" (_not implemented_).

It is highly recommended to use literals whenever possible because the compiler
can optimise them in some cases (especially church numerals).

##### Characters

Character literals are the same as a number literal with the ASCII value of that
character. So for example `'0'` is the same as `48`. Characters are always scott
encoded numbers.

##### Strings

Strings don't really exists in Musubi. There actually isn't even a token for
strings. String literals get expanded by the tokeniser so `"Hello"` becomes
`('H' : 'e' : 'l' : 'l' : 'o' : nil)`. This obviously requires the cons operator
`:` and `nil` to be defined (which `stdlib/lists.mbim` does but any other valid
definition is okay as well).

#### The #define directive

Also inspired by C is the #define directive. It works similar to the way it
works in C but it cannot take parameters. The name of a macro can be any
identifier. Everything after the identifier on the line that has #define on it
will be tokenised and put in as replacement for any occurrence of that
identifier.

##### Example

```
#define -> . ; defines `->` as `.` so you can now write `\a -> b` instead of `\a.b`
```

## Writing Modules

Modules are Musubi files that only provide definitions. The suggested format of
modules is

```
#once
#include dependency1
#include dependency2
...

let
  {definitions}
in
```

You'll notice that this isn't a valid Musubi program (no expression after `in`).
But writing a module in this way allows a module to be included at the top of a
file, similar to C:

```
#include {module}

; I now have access to all the definitions provided by {module}
definition1 definition2
```

Modules should use the file extension `mbim` instead of `mbi` so you can
immediately see that it's a module. This may also allow a potential LSP server
to ignore the error for the missing expression after `in` based on the file
extension.

## The rest

Musubi intentionally doesn't have a lot of built-in features to stay as close to
LC as possible. The
[standard library](https://github.com/Schlafhase/MusubiStdlib), which is written
in Musubi, includes a lot more features, like the `if`, `then`, `else`, `end`
macros, that simplify programming a lot.
