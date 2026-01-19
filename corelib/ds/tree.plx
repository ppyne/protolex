# protolex-corelib — ds/tree
# Version 0.1

import Array from "array.plx"
import List from "list.plx"

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

Tree.of = fn(items, compare) {
    if items.proto == Tree {
        items
    } else if items.proto == Array {
        t = Tree.empty()
        i = 0
        n = Array.length(items)
        while i < n {
            Tree.insert(t, Array.get(items, i), compare)
            i = i + 1
        }
        t
    } else if items.proto == List {
        t = Tree.empty()
        cur = items
        while !List.isNil(cur) {
            Tree.insert(t, List.head(cur), compare)
            cur = List.tail(cur)
        }
        t
    } else {
        throw "Tree.of expects Array or List"
    }
}

freeze(Tree)

Tree
