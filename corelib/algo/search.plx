# protolex-corelib — algo/search
# Version 0.1

import ds from "../ds/index.plx"

Search = [
    proto = null
]

Search.linear = fn(arr, value) {
    i = 0
    n = ds.Array.length(arr)

    while i < n {
        if ds.Array.get(arr, i) == value {
            i
        }
        i = i + 1
    }

    null
}

Search.binary = fn(arr, value, less) {
    low = 0
    high = ds.Array.length(arr) - 1

    while low <= high {
        mid = (low + high) / 2
        midval = ds.Array.get(arr, mid)

        if midval == value {
            mid
        } else if less(midval, value) {
            low = mid + 1
        } else {
            high = mid - 1
        }
    }

    null
}

Search.bfs = fn(g, start) {
    if !ds.Graph.hasNode(g, start) {
        ds.Array.new()
    } else {
        visited = ds.Set.new()
        order = ds.Array.new()
        q = ds.Queue.new()

        ds.Set.add(visited, start)
        ds.Queue.enqueue(q, start)
        ds.Array.push(order, start)

        while q.head < ds.Array.length(q._impl) {
            node = ds.Queue.dequeue(q)
            neighbors = ds.Graph.neighbors(g, node)
            i = 0
            n = ds.Array.length(neighbors)
            while i < n {
                nb = ds.Array.get(neighbors, i)
                if !ds.Set.contains(visited, nb) {
                    ds.Set.add(visited, nb)
                    ds.Queue.enqueue(q, nb)
                    ds.Array.push(order, nb)
                }
                i = i + 1
            }
        }
        order
    }
}

freeze(Search)

Search
