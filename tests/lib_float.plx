import float from "runtime/float"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

assert(float.abs(-3.5) == 3.5, "abs")
assert(float.min(1.0, 2.0) == 1.0, "min")
assert(float.max(1.0, 2.0) == 2.0, "max")
assert(float.round(3.5) == 4.0, "round")
assert(float.floor(3.9) == 3.0, "floor")
assert(float.ceil(3.1) == 4.0, "ceil")
assert(float.pow(2.0, 3.0) == 8.0, "pow")
assert(float.sqrt(9.0) == 3.0, "sqrt")
assert(float.parse("3.5") == 3.5, "parse")
assert(float.toString(3.5) == "3.5", "toString")
