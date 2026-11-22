# Compiler reference

This document isn't really documentation. I am writing it to have a place for
all the quirks and special behaviours of the compiler.

## The literal field in the Token record

Here is a table for all tokens for which the value of the `Literal` field is not
obvious:

| TokenType    | Literal                                           |
| ------------ | ------------------------------------------------- |
| BeginInclude | The included file as it was written in the source |
| BeginMacro   | The macro name                                    |
| Identifier   | The Identifier name                               |

`Literal` stays `null` for most tokens.
