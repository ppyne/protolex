assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

proto = [
    proto = null,
    x = 1,
    y = 2
]

obj = clone(proto)

mutate obj {
    obj.x = 10
    undefine obj.y
}

assert(obj.x == 10, "mutate")
assert(has(obj, "y") == false, "undefine local")
assert(isAbsent(obj.y) == false, "undefine lookup fallback")
