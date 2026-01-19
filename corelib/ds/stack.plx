# protolex-corelib — ds/stack
# Version 0.1

import Array from "array.plx"
import List from "list.plx"

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

Stack.of = fn(items) {
    if items.proto == Stack {
        items
    } else if items.proto == Array {
        st = Stack.new()
        i = 0
        n = Array.length(items)
        while i < n {
            Stack.push(st, Array.get(items, i))
            i = i + 1
        }
        st
    } else if items.proto == List {
        st = Stack.new()
        cur = items
        while !List.isNil(cur) {
            Stack.push(st, List.head(cur))
            cur = List.tail(cur)
        }
        st
    } else {
        throw "Stack.of expects Array or List"
    }
}

freeze(Stack)

Stack
