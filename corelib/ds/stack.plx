# protolex-corelib — ds/stack
# Version 0.1

import Array from "array.plx"

Stack = [
    proto = null
]

Stack.new = fn() {
    Stack = [
        proto = Stack,
        _impl = Array.new()
    ]
    Stack
}

Stack.push = fn(st, value) {
    Array.push(st._impl, value)
}

Stack.pop = fn(st) {
    Array.pop(st._impl)
}

Stack.peek = fn(st) {
    if Array.length(st._impl) == 0 {
        throw "Stack.peek on empty"
    }
    Array.get(st._impl, Array.length(st._impl) - 1)
}

freeze(Stack)

Stack
