assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

obj = [
    proto = null,
    x = 1
]

assert(isAbsent(obj.missing) == true, "absence")
