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
    Array.push(data, value)
    shift = fn(i) {
        if i < 0 {
            Array.set(data, 0, value)
        } else if compare(value, Array.get(data, i)) {
            Array.set(data, i + 1, Array.get(data, i))
            shift(i - 1)
        } else {
            Array.set(data, i + 1, value)
        }
    }
    shift(n - 1)
}

Tree.contains = fn(t, value, compare) {
    data = t._impl.data
    loop = fn(i) {
        if i >= Array.length(data) {
            false
        } else {
            v = Array.get(data, i)
            if !compare(v, value) && !compare(value, v) {
                true
            } else {
                loop(i + 1)
            }
        }
    }
    loop(0)
}

Tree.remove = fn(t, value, compare) {
    data = t._impl.data
    shift_left = fn(j) {
        if j >= Array.length(data) - 1 {
            Array.pop(data)
        } else {
            Array.set(data, j, Array.get(data, j + 1))
            shift_left(j + 1)
        }
    }
    loop = fn(i) {
        if i >= Array.length(data) {
            null
        } else {
            v = Array.get(data, i)
            if !compare(v, value) && !compare(value, v) {
                shift_left(i)
            } else {
                loop(i + 1)
            }
        }
    }
    loop(0)
}

Tree.of = fn(items, compare) {
    if items.proto == Tree {
        items
    } else if items.proto == Array {
        t = Tree.empty()
        loop = fn(i) {
            if i < Array.length(items) {
                Tree.insert(t, Array.get(items, i), compare)
                loop(i + 1)
            } else {
                t
            }
        }
        loop(0)
    } else if items.proto == List {
        t = Tree.empty()
        loop = fn(lst) {
            if List.isNil(lst) {
                t
            } else {
                Tree.insert(t, List.head(lst), compare)
                loop(List.tail(lst))
            }
        }
        loop(items)
    } else {
        throw "Tree.of expects Array or List"
    }
}

freeze(Tree)

Tree
