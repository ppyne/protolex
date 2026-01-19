# protolex-corelib — algo/search
# Version 0.1

import ds from "../ds/index.plx"

Search = [
    proto = null
]

Search.linear = fn(arr, value) {
    n = ds.Array.length(arr)
    loop = fn(i) {
        if i >= n {
            null
        } else if ds.Array.get(arr, i) == value {
            i
        } else {
            loop(i + 1)
        }
    }
    loop(0)
}

Search.binary = fn(arr, value, less) {
    loop = fn(low, high) {
        if low > high {
            null
        } else {
            mid = (low + high) / 2
            midval = ds.Array.get(arr, mid)

            if midval == value {
                mid
            } else if less(midval, value) {
                loop(mid + 1, high)
            } else {
                loop(low, mid - 1)
            }
        }
    }
    loop(0, ds.Array.length(arr) - 1)
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

        visit_neighbors = fn(neighbors, i) {
            if i >= ds.Array.length(neighbors) {
                null
            } else {
                nb = ds.Array.get(neighbors, i)
                if !ds.Set.contains(visited, nb) {
                    ds.Set.add(visited, nb)
                    ds.Queue.enqueue(q, nb)
                    ds.Array.push(order, nb)
                }
                visit_neighbors(neighbors, i + 1)
            }
        }

        loop = fn() {
            if q.head >= ds.Array.length(q._impl) {
                order
            } else {
                node = ds.Queue.dequeue(q)
                neighbors = ds.Graph.neighbors(g, node)
                visit_neighbors(neighbors, 0)
                loop()
            }
        }
        loop()
    }
}

freeze(Search)

Search
