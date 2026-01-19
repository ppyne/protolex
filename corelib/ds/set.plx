# protolex-corelib — ds/set
# Version 0.1

import Map from "map.plx"

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

freeze(Set)

Set
