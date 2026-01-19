# protolex-corelib — ds/map
# Version 0.1

Map = [
    proto = null
]

Map.new = fn() {
    internal = [
        proto = null,
        table = []
    ]

    m = [
        proto = Map,
        _impl = internal
    ]

    m
}

Map.put = fn(m, key, value) {
    mutate m._impl {
        m._impl.table[key] = value
    }
}

Map.get = fn(m, key) {
    m._impl.table[key]
}

Map.has = fn(m, key) {
    has(m._impl.table, key)
}

Map.remove = fn(m, key) {
    mutate m._impl {
        undefine m._impl.table[key]
    }
}

Map.size = fn(m) {
    # taille calculée dynamiquement
    count = 0
    for k in m._impl.table {
        count = count + 1
    }
    count
}

Map.of = fn(items) {
    if items.proto == Map {
        items
    } else {
        m = Map.new()
        for k in items {
            Map.put(m, k, items[k])
        }
        m
    }
}

freeze(Map)

Map
