# protolex-corelib — algo/sort
# Version 0.1

import ds from "../ds/index.plx"

Sort = [
    proto = null
]

Sort.insertion = fn(arr, less) {
    n = ds.Array.length(arr)

    i = 1
    while i < n {
        key = ds.Array.get(arr, i)
        j = i - 1

        while j >= 0 && less(key, ds.Array.get(arr, j)) {
            ds.Array.set(arr, j + 1, ds.Array.get(arr, j))
            j = j - 1
        }

        ds.Array.set(arr, j + 1, key)
        i = i + 1
    }
}

Sort.selection = fn(arr, less) {
    n = ds.Array.length(arr)
    i = 0

    while i < n - 1 {
        min = i
        j = i + 1

        while j < n {
            if less(ds.Array.get(arr, j), ds.Array.get(arr, min)) {
                min = j
            }
            j = j + 1
        }

        if min != i {
            tmp = ds.Array.get(arr, i)
            ds.Array.set(arr, i, ds.Array.get(arr, min))
            ds.Array.set(arr, min, tmp)
        }

        i = i + 1
    }
}

Sort.quick = fn(arr, less) {

    partition = fn(low, high) {
        pivot = ds.Array.get(arr, high)
        i = low
        j = low

        while j < high {
            if less(ds.Array.get(arr, j), pivot) {
                tmp = ds.Array.get(arr, i)
                ds.Array.set(arr, i, ds.Array.get(arr, j))
                ds.Array.set(arr, j, tmp)
                i = i + 1
            }
            j = j + 1
        }

        tmp = ds.Array.get(arr, i)
        ds.Array.set(arr, i, ds.Array.get(arr, high))
        ds.Array.set(arr, high, tmp)

        i
    }

    qs = fn(low, high) {
        if low < high {
            p = partition(low, high)
            qs(low, p - 1)
            qs(p + 1, high)
        }
    }

    n = ds.Array.length(arr)
    if n > 1 {
        qs(0, n - 1)
    }
}

freeze(Sort)

Sort
