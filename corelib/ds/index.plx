# protolex-corelib — ds/index
# Version 0.1

import List  from "list.plx"
import Array from "array.plx"
import Map   from "map.plx"
import Set   from "set.plx"
import Stack from "stack.plx"
import Queue from "queue.plx"
import Tree  from "tree.plx"
import Graph from "graph.plx"

ds = [
    List  = List,
    Array = Array,
    Map   = Map,
    Set   = Set,
    Stack = Stack,
    Queue = Queue,
    Tree  = Tree,
    Graph = Graph
]

freeze(ds)

mutate List {
    List.of = fn(items) {
        if items.proto == List {
            items
        } else if items.proto == Array {
            i = Array.length(items) - 1
            acc = List.nil
            while i >= 0 {
                acc = List.cons(Array.get(items, i), acc)
                i = i - 1
            }
            acc
        } else {
            throw "List.of expects Array or List"
        }
    }
}

mutate Array {
    Array.of = fn(items) {
        if items.proto == Array {
            items
        } else if items.proto == List {
            arr = Array.new()
            cur = items
            while !List.isNil(cur) {
                Array.push(arr, List.head(cur))
                cur = List.tail(cur)
            }
            arr
        } else {
            throw "Array.of expects Array or List"
        }
    }
}

ds
