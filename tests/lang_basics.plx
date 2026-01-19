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
