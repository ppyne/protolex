# protolex-corelib — Specification 0.1

## 0. Goal

`protolex-corelib` defines a **minimal but extensible** set of data structures and algorithms provided by the protolex runtime.

The corelib:

- **adds no rule to the language**

- introduces **no keyword**

- strictly respects:

  - freeze by default

  - controlled mutation

  - the absence of globals

  - the absence of reflection

All structures are exposed as **ordinary frozen objects**.

## Absence predicate

The language provides a standard predicate `isAbsent(expr)` to test whether
evaluating `expr` resulted in a missing slot (lookup failed).

Absence is not a value and cannot be stored or compared.

---

## 1. General principles

### 1.1. No built-in structures in the language

Protolex defines **no built-in data structure** other than the table.

All structures below are:

- provided by the runtime

- accessible via import

- implemented as objects + functions

---

### 1.2. Freeze and encapsulation

- objects representing structures are **frozen**

- users **can never mutate directly** their invariants

- internal mutation is allowed **only** inside corelib functions

This guarantees:

- safety

- consistency

- no structural corruption

---

### 1.3. Naming and organization

The corelib is organized into **object namespaces**:

`ds    // data structures algo  // algorithms`

### 1.4. File organization

The corelib is split by topic into dedicated folders:

`corelib/ds/*.plx`

`corelib/algo/*.plx`

Entry points are:

`import ds   from "corelib/ds/index.plx"`

`import algo from "corelib/algo/index.plx"`

---

## 2. Fundamental data structures (`ds`)

---

## 2.1. List — immutable linked list

### 2.1.1. Nature

- persistent list

- immutable by default

- explicit recursive structure

### 2.1.2. Minimal interface

`ds.List.nil ds.List.cons(head, tail) ds.List.isNil(list) ds.List.head(list) ds.List.tail(list)`

### 2.1.3. Utility functions

`ds.List.length(list) ds.List.map(fn, list) ds.List.fold(fn, init, list) ds.List.reverse(list)`

### 2.1.4. Properties

- no mutation

- structural sharing

- controlled memory cost

- fully compatible with `clone`

---

## 2.2. Array — encapsulated dynamic array

### 2.2.1. Nature

- indexed sequential structure

- order guaranteed **by the structure**

- controlled internal mutation

### 2.2.2. Creation

`a = ds.Array.new()`

### 2.2.3. Access

`ds.Array.length(a) ds.Array.get(a, index) ds.Array.set(a, index, value)`

### 2.2.4. Modification

`ds.Array.push(a, value) ds.Array.pop(a) ds.Array.insert(a, index, value) ds.Array.remove(a, index)`

### 2.2.5. Guarantees

- object `a` remains frozen from the user's perspective

- all mutation is internal to the corelib

- out-of-bounds indices -> exception

---

## 2.3. Map — key -> value dictionary

### 2.3.1. Nature

- key -> value association

- implementation free (hash, tree, etc.)

- totally independent from the protolex table

### 2.3.2. Interface

`m = ds.Map.new() ds.Map.put(m, key, value) ds.Map.get(m, key) ds.Map.has(m, key) ds.Map.remove(m, key) ds.Map.size(m)`

---

## 2.4. Set — mathematical set

### 2.4.1. Nature

- set with no duplicates

- conceptually based on Map

### 2.4.2. Interface

`s = ds.Set.new() ds.Set.add(s, value) ds.Set.remove(s, value) ds.Set.contains(s, value) ds.Set.size(s)`

---

## 2.5. Stack / Queue / Deque

Classic sequential structures, explicit interfaces:

### Stack (LIFO)

`st = ds.Stack.new() ds.Stack.push(st, value) ds.Stack.pop(st) ds.Stack.peek(st)`

### Queue (FIFO)

`q = ds.Queue.new() ds.Queue.enqueue(q, value) ds.Queue.dequeue(q)`

---

## 2.6. Tree — trees

### 2.6.1. Binary tree (base)

`t = ds.Tree.empty() ds.Tree.insert(t, value, compare) ds.Tree.contains(t, value, compare) ds.Tree.remove(t, value, compare)`

### 2.6.2. Possible variants (optional)

- AVL

- Red-Black

- B-Tree

Each variant is exposed as a **distinct object**.

---

## 2.7. Graph — graphs

### 2.7.1. Nature

- directed or undirected graph

- internal representation is free

### 2.7.2. Minimal interface

`g = ds.Graph.new() ds.Graph.addNode(g, node) ds.Graph.addEdge(g, from, to) ds.Graph.neighbors(g, node)`

---

## 3. Algorithms (`algo`)

Algorithms are **separate from structures**.

---

## 3.1. Sorting (`algo.Sort`)

Each algorithm is explicitly named.

`algo.Sort.quick(array, compare) algo.Sort.merge(array, compare) algo.Sort.heap(array, compare) algo.Sort.insertion(array, compare)`

- no implicit sort

- `compare(a, b)` returns bool or order

---

## 3.2. Search (`algo.Search`)

`algo.Search.linear(array, value) algo.Search.binary(array, value, compare) algo.Search.tree(tree, value, compare)`

---

## 3.3. Traversal and graphs

`algo.Search.bfs(graph, start) algo.Search.dfs(graph, start) algo.Search.dijkstra(graph, start) algo.Search.astar(graph, start, heuristic)`

---

## 4. Errors and exceptions

- any invariant violation -> `throw`

- no special return value

- no silent fallback

- exceptions cross calls normally

---

## 5. What corelib does not do

The corelib **does not provide**:

- concurrency

- parallelism

- persistence

- serialization

- I/O

- memory management

- typing

These aspects belong to the runtime or separate libraries.

---

## 6. Extension and compatibility

- any new structure must:

  - be an ordinary object

  - respect freezing

  - make all mutation explicit

- no extension may modify the language spec

---

## 7. Status

This document defines **protolex-corelib 0.1**.
It is strictly compatible with **protolex 0.1** and can evolve independently.

---

## Simplified collection construction

To ease the construction of data structures without introducing new syntax or dedicated literals,
the corelib provides explicit constructor functions.

### List.of

```protolex
ds.List.of(listOrArray)
```

Builds an immutable list from a list or an array.

### Array.of

```protolex
ds.Array.of(listOrArray)
```

Builds a dynamic array from a list or an array.

### Map.of

```protolex
ds.Map.of(tableOrMap)
```

Builds a map from a map or any table-like object by copying its slots.

### Set.of

```protolex
ds.Set.of(listOrArrayOrTableOrSet)
```

Builds a set from a set, list, array, or table (keys are used).

### Stack.of

```protolex
ds.Stack.of(listOrArray)
```

Builds a stack by pushing items in order.

### Queue.of

```protolex
ds.Queue.of(listOrArray)
```

Builds a queue by enqueuing items in order.

### Tree.of

```protolex
ds.Tree.of(listOrArray, compare)
```

Builds a tree by inserting items in order using `compare`.

### Graph.of

```protolex
ds.Graph.of(adjacency)
```

Builds a graph from an adjacency table where each key maps to a list or array of neighbors.

These functions replace any attempt at list or map literals, which are intentionally absent from the language.

---

## Absence of overloadable operators

The corelib does not redefine or overload **any syntactic operator** of the language.

All operations (arithmetic, algorithmic, structural)
are exposed as explicit named functions
(`Sort.quick`, `List.map`, `Map.put`, etc.).

---
