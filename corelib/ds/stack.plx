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
        loop = fn(i) {
            if i < Array.length(items) {
                Stack.push(st, Array.get(items, i))
                loop(i + 1)
            } else {
                st
            }
        }
        loop(0)
    } else if items.proto == List {
        st = Stack.new()
        loop = fn(lst) {
            if List.isNil(lst) {
                st
            } else {
                Stack.push(st, List.head(lst))
                loop(List.tail(lst))
            }
        }
        loop(items)
    } else {
        throw "Stack.of expects Array or List"
    }
}

freeze(Stack)

Stack
