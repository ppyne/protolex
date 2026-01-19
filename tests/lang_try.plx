assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

state = [
    proto = null,
    caught = false,
    done = false
]

danger = fn() {
    throw "boom"
}

try {
    danger()
} catch e {
    mutate state {
        state.caught = true
    }
} finally {
    mutate state {
        state.done = true
    }
}

assert(state.caught == true, "catch")
assert(state.done == true, "finally")
