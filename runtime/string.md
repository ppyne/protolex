# runtime/string — String

## Description

`string` provides minimal, explicit string utilities.

## Public interface

```protolex
import string from "runtime/string"

string.length(s)                -> int
string.concat(a, b)             -> string
string.slice(s, start, end)     -> string
string.split(s, sep)            -> list
string.charAt(s, i)             -> string
string.forEach(s, f)            -> null
string.indexOf(s, sub)          -> int
string.startsWith(s, prefix)    -> bool
string.endsWith(s, suffix)      -> bool
string.repeat(s, n)             -> string
string.toInt(s)                 -> int
string.toFloat(s)               -> float
string.format(fmt, ...)         -> string
```

## Semantic rules

- `slice` uses byte offsets, `start` inclusive and `end` exclusive
- `split` returns a list-like structure (`isNil`, `head`, `tail`)
- `charAt` returns a single-character string (byte)
- `forEach` calls `f(char, index)` for each single-character string (byte) in order
- `indexOf` returns `-1` if not found
- `repeat` repeats `s` `n` times, with `n` non-negative
- parsing errors raise exceptions
- `format` follows `sprintf`-style formatting for `%d/%i/%u/%x/%X/%o/%c/%f/%e/%g/%s` and `%%`

## Examples

```protolex
import string from "runtime/string"

len = string.length("abc")
joined = string.concat("a", "b")
sub = string.slice("hello", 1, 4)
parts = string.split("a,b,c", ",")
ch = string.charAt("hello", 1)
string.forEach("hi", fn(c, i) { c })
repeated = string.repeat("ha", 3)
msg = string.format("id=%d name=%s", 7, "proto")
```

## Possible errors

- invalid arguments
- invalid ranges for `slice`
- empty separator for `split`
- parse errors in `toInt`/`toFloat`
