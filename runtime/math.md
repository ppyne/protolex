# runtime/math — Math

## Description

`math` provides explicit mathematical functions and constants.

## Public interface

```protolex
import math from "runtime/math"

math.sin(x)     -> float
math.cos(x)     -> float
math.tan(x)     -> float
math.exp(x)     -> float
math.log(x)     -> float
math.random()   -> float
math.pi         -> float
math.e          -> float
```

## Semantic rules

- all inputs are floats
- NaN and Infinity are allowed
- `random` returns a float in [0, 1]

## Examples

```protolex
import math from "runtime/math"

pi = math.pi
x = math.sin(0.0)
```

## Possible errors

- invalid arguments
