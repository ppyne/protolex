![Protolex](header.png)

# Protolex Corelib — User Guide 0.1

This document explains how to use corelib in practice.
It focuses on concrete operations and examples.

## 1. Imports and entry points

```protolex
import ds from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"
```

Corelib provides:
- data structures under `ds`
- algorithms under `algo`

## 2. Iteration (no loops)

There are no `for`/`while`. You iterate with library functions or recursion.

```protolex
import ds from "corelib/ds/index.plx"

list = ds.List.of([1, 2, 3])

sum = ds.List.fold(fn(acc, x) { acc + x }, 0, list)
```

## 3. Lists (`ds.List`)

Lists are immutable linked lists. You build them with `cons` and `nil`, or with `List.of`.

### 3.1. Constructors

- `ds.List.nil`
- `ds.List.cons(head, tail)`
- `ds.List.of(listOrArray)`

Example:

```protolex
import ds from "corelib/ds/index.plx"

list = ds.List.cons(1,
       ds.List.cons(2,
       ds.List.cons(3,
       ds.List.nil)))

list2 = ds.List.of([1, 2, 3])
```

### 3.2. Access

- `ds.List.isNil(list)`
- `ds.List.head(list)`
- `ds.List.tail(list)`

Example:

```protolex
if ds.List.isNil(list) {
    "empty"
} else {
    ds.List.head(list)
}
```

### 3.3. Iteration and transform

- `ds.List.forEach(f, list)`
- `ds.List.map(f, list)`
- `ds.List.fold(f, acc, list)`
- `ds.List.filter(pred, list)`
- `ds.List.length(list)`
- `ds.List.reverse(list)`

Example (map + fold):

```protolex
doubles = ds.List.map(fn(x) { x * 2 }, list)
count = ds.List.length(doubles)
```

### 3.4. Print a list

```protolex
import io from "runtime/io"
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

list = ds.List.of([1, 2, 3])

emit = fn(x) {
    io.write(io.stdout, string.format("%d\n", x))
}

ds.List.forEach(emit, list)
```

## 4. Arrays (`ds.Array`)

Arrays are indexed structures with internal mutation (encapsulated).

### 4.1. Constructors

- `ds.Array.new()`
- `ds.Array.of(listOrArray)`

Example:

```protolex
arr = ds.Array.new()

ds.Array.push(arr, 10)
ds.Array.push(arr, 20)
```

### 4.2. Access and update

- `ds.Array.length(arr)`
- `ds.Array.get(arr, index)`
- `ds.Array.set(arr, index, value)`
- `ds.Array.push(arr, value)`
- `ds.Array.pop(arr)`

### 4.3. Iteration

- `ds.Array.forEach(f, arr)`
- `ds.Array.map(f, arr)`
- `ds.Array.fold(f, acc, arr)`

Example:

```protolex
sum = ds.Array.fold(fn(acc, x) { acc + x }, 0, arr)
```

## 5. Maps (`ds.Map`)

Maps are key/value dictionaries independent from tables.

### 5.1. Constructors

- `ds.Map.new()`
- `ds.Map.of(pairs)` where pairs are `{ key, value }`

Example:

```protolex
m = ds.Map.new()

ds.Map.put(m, "user", "ava")
value = ds.Map.get(m, "user")
exists = ds.Map.has(m, "user")
```

### 5.2. API

- `ds.Map.put(m, key, value)`
- `ds.Map.get(m, key)`
- `ds.Map.has(m, key)`
- `ds.Map.remove(m, key)`
- `ds.Map.size(m)`

## 6. Sets (`ds.Set`)

Sets store unique values.

### 6.1. Constructors

- `ds.Set.new()`
- `ds.Set.of(listOrArrayOrSet)`

### 6.2. API

- `ds.Set.add(s, value)`
- `ds.Set.remove(s, value)`
- `ds.Set.contains(s, value)`
- `ds.Set.size(s)`

## 7. Stack (`ds.Stack`) and Queue (`ds.Queue`)

### Stack (LIFO)

- `ds.Stack.new()`
- `ds.Stack.push(st, value)`
- `ds.Stack.pop(st)`
- `ds.Stack.peek(st)`
- `ds.Stack.of(listOrArray)`

### Queue (FIFO)

- `ds.Queue.new()`
- `ds.Queue.enqueue(q, value)`
- `ds.Queue.dequeue(q)`
- `ds.Queue.of(listOrArray)`

## 8. Tree (`ds.Tree`)

Trees are ordered by a `compare(a, b)` function.

### Constructors

- `ds.Tree.empty()`
- `ds.Tree.of(listOrArray, compare)`

### API

- `ds.Tree.insert(t, value, compare)`
- `ds.Tree.contains(t, value, compare)`
- `ds.Tree.remove(t, value, compare)`

Example:

```protolex
compare = fn(a, b) { a < b }

t = ds.Tree.empty()

t = ds.Tree.insert(t, 3, compare)

t = ds.Tree.insert(t, 1, compare)

has3 = ds.Tree.contains(t, 3, compare)
```

## 9. Graph (`ds.Graph`)

Graphs are built from edges `{ from, to }`.

### Constructors

- `ds.Graph.new()`
- `ds.Graph.of(edges)` where edges are list/array of `{ from, to }`

### API

- `ds.Graph.addNode(g, node)`
- `ds.Graph.addEdge(g, from, to)`
- `ds.Graph.neighbors(g, node)`
- `ds.Graph.hasNode(g, node)`

Example:

```protolex
edges = ds.Array.new()

ds.Array.push(edges, [ from = "a", to = "b" ])
ds.Array.push(edges, [ from = "b", to = "c" ])

g = ds.Graph.of(edges)

nbs = ds.Graph.neighbors(g, "b")
```

## 10. Algorithms (`algo`)

Algorithms are separate from structures.

### 10.1. Sorting (`algo.Sort`)

- `algo.Sort.insertion(arr, less)`
- `algo.Sort.selection(arr, less)`
- `algo.Sort.quick(arr, less)`

Example:

```protolex
import ds from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"

arr = ds.Array.of([5, 2, 9, 1])
less = fn(a, b) { a < b }

sorted = algo.Sort.quick(arr, less)
```

### 10.2. Search (`algo.Search`)

- `algo.Search.linear(arr, value)`
- `algo.Search.binary(arr, value, less)`
- `algo.Search.bfs(graph, start)`

Example:

```protolex
index = algo.Search.linear(arr, 9)
```

## 11. Notes

- There are no language-level loops; use `List.*`, `Array.*`, or recursion.
- Construction helpers like `List.of` and `Array.of` are the easiest way to initialize data.
- Data structures expose explicit APIs; there is no implicit iteration on tables.
