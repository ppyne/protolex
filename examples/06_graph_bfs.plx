# Exemple 06 — Graphe et parcours BFS

import ds   from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"

g = ds.Graph.new()

ds.Graph.addNode(g, "A")
ds.Graph.addNode(g, "B")
ds.Graph.addNode(g, "C")

ds.Graph.addEdge(g, "A", "B")
ds.Graph.addEdge(g, "A", "C")

visited = algo.Search.bfs(g, "A")
