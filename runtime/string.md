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
string.indexOf(s, sub)          -> int
string.startsWith(s, prefix)    -> bool
string.endsWith(s, suffix)      -> bool
string.toInt(s)                 -> int
string.toFloat(s)               -> float
```

## Semantic rules

- `slice` uses byte offsets, `start` inclusive and `end` exclusive
- `split` returns a list-like structure (`isNil`, `head`, `tail`)
- `indexOf` returns `-1` if not found
- parsing errors raise exceptions

## Examples

```protolex
import string from "runtime/string"

len = string.length("abc")
joined = string.concat("a", "b")
sub = string.slice("hello", 1, 4)
parts = string.split("a,b,c", ",")
```

## Possible errors

- invalid arguments
- invalid ranges for `slice`
- empty separator for `split`
- parse errors in `toInt`/`toFloat`
