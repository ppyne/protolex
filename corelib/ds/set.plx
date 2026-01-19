# protolex-corelib — ds/set
# Version 0.1

import Map from "map.plx"
import Array from "array.plx"
import List from "list.plx"

Set = [
    proto = null
]

Set.new = fn() {
    internal = [
        proto = null,
        map = Map.new()
    ]

    s = [
        proto = Set,
        _impl = internal
    ]

    s
}

Set.add = fn(s, value) {
    Map.put(s._impl.map, value, true)
}

Set.remove = fn(s, value) {
    Map.remove(s._impl.map, value)
}

Set.contains = fn(s, value) {
    Map.has(s._impl.map, value)
}

Set.size = fn(s) {
    Map.size(s._impl.map)
}

Set.of = fn(items) {
    if items.proto == Set {
        items
    } else if items.proto == Array {
        s = Set.new()
        i = 0
        n = Array.length(items)
        while i < n {
            Set.add(s, Array.get(items, i))
            i = i + 1
        }
        s
    } else if items.proto == List {
        s = Set.new()
        cur = items
        while !List.isNil(cur) {
            Set.add(s, List.head(cur))
            cur = List.tail(cur)
        }
        s
    } else {
        s = Set.new()
        for k in items {
            Set.add(s, k)
        }
        s
    }
}

freeze(Set)

Set
