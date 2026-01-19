# runtime/float — Float

## Description

`float` provides explicit floating-point utilities.

## Public interface

```protolex
import float from "runtime/float"

float.abs(x)       -> float
float.min(a, b)    -> float
float.max(a, b)    -> float
float.round(x)     -> float
float.floor(x)     -> float
float.ceil(x)      -> float
float.pow(x, y)    -> float
float.sqrt(x)      -> float
float.parse(string) -> float
float.toString(x)  -> string
```

## Semantic rules

- NaN and Infinity are allowed
- parsing errors raise exceptions

## Examples

```protolex
import float from "runtime/float"

value = float.sqrt(9.0)
text = float.toString(3.5)
```

## Possible errors

- invalid arguments
- parse errors
