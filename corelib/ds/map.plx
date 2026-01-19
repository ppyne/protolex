# protolex-corelib — ds/map
# Version 0.1

import Array from "array.plx"
import List from "list.plx"

Map = [
    proto = null
]

Map.new = fn() {
    internal = [
        proto = null,
        table = [],
        size = 0
    ]

    m = [
        proto = Map,
        _impl = internal
    ]

    m
}

Map.put = fn(m, key, value) {
    existed = has(m._impl.table, key)
    mutate m._impl {
        m._impl.table[key] = value
        if !existed {
            m._impl.size = m._impl.size + 1
        }
    }
}

Map.get = fn(m, key) {
    m._impl.table[key]
}

Map.has = fn(m, key) {
    has(m._impl.table, key)
}

Map.remove = fn(m, key) {
    existed = has(m._impl.table, key)
    mutate m._impl {
        undefine m._impl.table[key]
        if existed {
            m._impl.size = m._impl.size - 1
        }
    }
}

Map.size = fn(m) {
    m._impl.size
}

Map.of = fn(items) {
    if items.proto == Map {
        items
    } else if items.proto == Array {
        m = Map.new()
        loop = fn(i) {
            if i < Array.length(items) {
                pair = Array.get(items, i)
                Map.put(m, pair.key, pair.value)
                loop(i + 1)
            } else {
                m
            }
        }
        loop(0)
    } else {
        m = Map.new()
        loop = fn(lst) {
            if List.isNil(lst) {
                m
            } else {
                pair = List.head(lst)
                Map.put(m, pair.key, pair.value)
                loop(List.tail(lst))
            }
        }
        loop(items)
    }
}

freeze(Map)

Map
