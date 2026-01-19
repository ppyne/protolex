# runtime/int — Int

## Description

`int` provides explicit integer utilities.

## Public interface

```protolex
import int from "runtime/int"

int.abs(x)         -> int
int.min(a, b)      -> int
int.max(a, b)      -> int
int.clamp(x, min, max) -> int
int.pow(x, n)      -> int
int.parse(string)  -> int
int.toString(x)    -> string
```

## Semantic rules

- integer overflow behavior follows the host runtime
- `pow` expects a non-negative exponent
- parsing errors raise exceptions

## Examples

```protolex
import int from "runtime/int"

value = int.clamp(10, 0, 5)
text = int.toString(42)
```

## Possible errors

- invalid arguments
- invalid ranges
- parse errors
