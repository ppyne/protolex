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
        loop = fn(i) {
            if i < Array.length(items) {
                Set.add(s, Array.get(items, i))
                loop(i + 1)
            } else {
                s
            }
        }
        loop(0)
    } else if items.proto == List {
        s = Set.new()
        loop = fn(lst) {
            if List.isNil(lst) {
                s
            } else {
                Set.add(s, List.head(lst))
                loop(List.tail(lst))
            }
        }
        loop(items)
    } else {
        throw "Set.of expects Array, List, or Set"
    }
}

freeze(Set)

Set
