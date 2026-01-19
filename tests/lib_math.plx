import math from "runtime/math"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

assert(math.sin(0.0) == 0.0, "sin")
assert(math.cos(0.0) == 1.0, "cos")
assert(math.tan(0.0) == 0.0, "tan")
assert(math.exp(0.0) == 1.0, "exp")
assert(math.log(1.0) == 0.0, "log")
assert(math.pi > 3.14, "pi")
assert(math.e > 2.71, "e")
