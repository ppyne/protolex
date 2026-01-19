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
            loop = fn(i, acc) {
                if i < 0 {
                    acc
                } else {
                    loop(i - 1, List.cons(Array.get(items, i), acc))
                }
            }
            loop(Array.length(items) - 1, List.nil)
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
            loop = fn(lst) {
                if List.isNil(lst) {
                    arr
                } else {
                    Array.push(arr, List.head(lst))
                    loop(List.tail(lst))
                }
            }
            loop(items)
        } else {
            throw "Array.of expects Array or List"
        }
    }
}

ds
