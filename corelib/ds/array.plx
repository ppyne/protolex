# protolex-corelib — ds/array
# Version 0.1

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

Array
