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

ds
