assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

proto = [
    proto = null,
    x = 7
]

obj = clone(proto)
assert(obj.x == 7, "proto lookup")

assert(has(obj, "x") == false, "has local")
