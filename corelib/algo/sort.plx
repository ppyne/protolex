# protolex-corelib — algo/sort
# Version 0.1

import ds from "../ds/index.plx"

Sort = [
    proto = null
]

Sort.insertion = fn(arr, less) {
    n = ds.Array.length(arr)
    insert_at = fn(i) {
        if i >= n {
            null
        } else {
            key = ds.Array.get(arr, i)
            shift = fn(j) {
                if j >= 0 && less(key, ds.Array.get(arr, j)) {
                    ds.Array.set(arr, j + 1, ds.Array.get(arr, j))
                    shift(j - 1)
                } else {
                    ds.Array.set(arr, j + 1, key)
                }
            }
            shift(i - 1)
            insert_at(i + 1)
        }
    }
    insert_at(1)
}

Sort.selection = fn(arr, less) {
    n = ds.Array.length(arr)
    find_min = fn(j, min) {
        if j >= n {
            min
        } else if less(ds.Array.get(arr, j), ds.Array.get(arr, min)) {
            find_min(j + 1, j)
        } else {
            find_min(j + 1, min)
        }
    }
    select = fn(i) {
        if i >= n - 1 {
            null
        } else {
            min = find_min(i + 1, i)
            if min != i {
                tmp = ds.Array.get(arr, i)
                ds.Array.set(arr, i, ds.Array.get(arr, min))
                ds.Array.set(arr, min, tmp)
            }
            select(i + 1)
        }
    }
    select(0)
}

Sort.quick = fn(arr, less) {

    partition = fn(low, high) {
        pivot = ds.Array.get(arr, high)
        swap = fn(a, b) {
            tmp = ds.Array.get(arr, a)
            ds.Array.set(arr, a, ds.Array.get(arr, b))
            ds.Array.set(arr, b, tmp)
        }
        walk = fn(j, i) {
            if j >= high {
                swap(i, high)
                i
            } else if less(ds.Array.get(arr, j), pivot) {
                swap(i, j)
                walk(j + 1, i + 1)
            } else {
                walk(j + 1, i)
            }
        }
        walk(low, low)
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
