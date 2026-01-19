import int from "runtime/int"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

assert(int.abs(-3) == 3, "abs")
assert(int.min(1, 2) == 1, "min")
assert(int.max(1, 2) == 2, "max")
assert(int.clamp(5, 0, 3) == 3, "clamp")
assert(int.pow(2, 3) == 8, "pow")
assert(int.parse("42") == 42, "parse")
assert(int.toString(42) == "42", "toString")
