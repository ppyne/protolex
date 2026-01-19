# protolex-corelib — ds/graph
# Version 0.1

import Map from "map.plx"
import Array from "array.plx"
import List from "list.plx"

Graph = [
    proto = null
]

Graph.new = fn() {
    internal = [
        proto = null,
        nodes = Map.new()
    ]
    g = [
        proto = Graph,
        _impl = internal
    ]
    g
}

Graph._neighbors = fn(g, node) {
    if Map.has(g._impl.nodes, node) {
        Map.get(g._impl.nodes, node)
    } else {
        null
    }
}

Graph.hasNode = fn(g, node) {
    Map.has(g._impl.nodes, node)
}

Graph.neighbors = fn(g, node) {
    n = Graph._neighbors(g, node)
    if n == null {
        Array.new()
    } else {
        n
    }
}

Graph.addNode = fn(g, node) {
    if !Map.has(g._impl.nodes, node) {
        Map.put(g._impl.nodes, node, Array.new())
    }
}

Graph._addEdgeOne = fn(g, a, b) {
    arr = Graph._neighbors(g, a)
    if arr == null {
        Graph.addNode(g, a)
        arr = Graph._neighbors(g, a)
    }
    Array.push(arr, b)
}

Graph.addEdge = fn(g, a, b) {
    Graph._addEdgeOne(g, a, b)
    Graph._addEdgeOne(g, b, a)
}

Graph.of = fn(adj) {
    if adj.proto == Graph {
        adj
    } else if adj.proto == Array {
        g = Graph.new()
        loop = fn(i) {
            if i < Array.length(adj) {
                edge = Array.get(adj, i)
                Graph.addEdge(g, edge.from, edge.to)
                loop(i + 1)
            } else {
                g
            }
        }
        loop(0)
    } else if adj.proto == List {
        g = Graph.new()
        loop = fn(lst) {
            if List.isNil(lst) {
                g
            } else {
                edge = List.head(lst)
                Graph.addEdge(g, edge.from, edge.to)
                loop(List.tail(lst))
            }
        }
        loop(adj)
    } else {
        throw "Graph.of expects Array, List, or Graph"
    }
}

freeze(Graph)

Graph
