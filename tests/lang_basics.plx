assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

a = 2 + 3
assert(a == 5, "math")

s = "ok"
assert(s == "ok", "string")

t = [
    proto = null,
    x = 1
]

assert(t.x == 1, "table lookup")

assert(isInt(1) == true, "isInt")
assert(isFloat(1.5) == true, "isFloat")
assert(isString("x") == true, "isString")
assert(isBool(true) == true, "isBool")
assert(isTable(t) == true, "isTable")
assert(isFunction(fn(x) { x }) == true, "isFunction")
