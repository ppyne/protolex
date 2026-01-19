# protolex-corelib — ds/tree
# Version 0.1

import Array from "array.plx"

Tree = [
    proto = null
]

Tree.empty = fn() {
    internal = [
        proto = null,
        data = Array.new()
    ]
    t = [
        proto = Tree,
        _impl = internal
    ]
    t
}

Tree.insert = fn(t, value, compare) {
    data = t._impl.data
    n = Array.length(data)
    i = n - 1
    Array.push(data, value)
    done = false
    while i >= 0 && !done {
        if compare(value, Array.get(data, i)) {
            Array.set(data, i + 1, Array.get(data, i))
            i = i - 1
        } else {
            done = true
        }
    }
    Array.set(data, i + 1, value)
}

Tree.contains = fn(t, value, compare) {
    data = t._impl.data
    i = 0
    n = Array.length(data)
    while i < n {
        v = Array.get(data, i)
        if !compare(v, value) && !compare(value, v) {
            true
        }
        i = i + 1
    }
    false
}

Tree.remove = fn(t, value, compare) {
    data = t._impl.data
    i = 0
    n = Array.length(data)
    while i < n {
        v = Array.get(data, i)
        if !compare(v, value) && !compare(value, v) {
            j = i
            while j < n - 1 {
                Array.set(data, j, Array.get(data, j + 1))
                j = j + 1
            }
            Array.pop(data)
            i = n
        } else {
            i = i + 1
        }
    }
}

freeze(Tree)

Tree
