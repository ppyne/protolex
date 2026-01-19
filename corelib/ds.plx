# protolex-corelib — ds
# Structures de données fondamentales
# Version 0.1

# ============================================================
# List — liste chaînée immuable
# ============================================================

List = [
    proto = null
]

List.nil = [
    proto = List,
    isNil = true
]

List.cons = fn(head, tail) {
    node = [
        proto = List,
        head = head,
        tail = tail,
        isNil = false
    ]
    node
}

List.isNil = fn(list) {
    list.isNil == true
}

List.head = fn(list) {
    if list.isNil {
        throw "List.head on nil"
    }
    list.head
}

List.tail = fn(list) {
    if list.isNil {
        throw "List.tail on nil"
    }
    list.tail
}

List.length = fn(list) {
    loop = fn(lst, acc) {
        if lst.isNil {
            acc
        } else {
            loop(lst.tail, acc + 1)
        }
    }
    loop(list, 0)
}

List.map = fn(f, list) {
    if list.isNil {
        List.nil
    } else {
        List.cons(
            f(list.head),
            List.map(f, list.tail)
        )
    }
}

List.fold = fn(f, acc, list) {
    if list.isNil {
        acc
    } else {
        List.fold(
            f,
            f(acc, list.head),
            list.tail
        )
    }
}

List.reverse = fn(list) {
    loop = fn(lst, acc) {
        if lst.isNil {
            acc
        } else {
            loop(lst.tail, List.cons(lst.head, acc))
        }
    }
    loop(list, List.nil)
}

freeze(List)
freeze(List.nil)

# ============================================================
# Array — tableau dynamique encapsulé
# ============================================================

Array = [
    proto = null
]

Array.new = fn() {
    internal = [
        proto = null,
        data = [],
        size = 0
    ]

    arr = [
        proto = Array,
        _impl = internal
    ]

    arr
}

Array.length = fn(arr) {
    arr._impl.size
}

Array.get = fn(arr, index) {
    if index < 0 || index >= arr._impl.size {
        throw "Array.get out of bounds"
    }
    arr._impl.data[index]
}

Array.set = fn(arr, index, value) {
    if index < 0 || index >= arr._impl.size {
        throw "Array.set out of bounds"
    }
    mutate arr._impl {
        arr._impl.data[index] = value
    }
}

Array.push = fn(arr, value) {
    mutate arr._impl {
        arr._impl.data[arr._impl.size] = value
        arr._impl.size = arr._impl.size + 1
    }
}

Array.pop = fn(arr) {
    if arr._impl.size == 0 {
        throw "Array.pop on empty"
    }
    value = arr._impl.data[arr._impl.size - 1]
    mutate arr._impl {
        undefine arr._impl.data[arr._impl.size - 1]
        arr._impl.size = arr._impl.size - 1
    }
    value
}

freeze(Array)

# ============================================================
# Map — dictionnaire clé → valeur
# ============================================================

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

freeze(Map)

# ============================================================
# Set — ensemble mathématique
# ============================================================

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

# ============================================================
# Stack — pile LIFO
# ============================================================

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

freeze(Stack)

# ============================================================
# Queue — file FIFO
# ============================================================

Queue = [
    proto = null
]

Queue.new = fn() {
    q = [
        proto = Queue,
        _impl = Array.new(),
        head = 0
    ]
    q
}

Queue.enqueue = fn(q, value) {
    Array.push(q._impl, value)
}

Queue.dequeue = fn(q) {
    if q.head >= Array.length(q._impl) {
        throw "Queue.dequeue on empty"
    }
    value = Array.get(q._impl, q.head)
    mutate q {
        q.head = q.head + 1
    }
    value
}

freeze(Queue)

# ============================================================
# Export
# ============================================================

ds = [
    List  = List,
    Array = Array,
    Map   = Map,
    Set   = Set,
    Stack = Stack,
    Queue = Queue
]

freeze(ds)

ds
