![Protolex](header.png)

# Protolex — User Manual

This manual is practical: what the language can do today, and how to use it.
Every section includes examples you can run.

## 1. Quick start

Run a script:

```bash
./protolex examples/01_list_basic.plx
```

Minimal program that prints to the terminal:

```protolex
import io from "runtime/io"

io.write(io.stdout, "hello\n")
```

## 2. Values and literals

Protolex has a small set of values:
- int, float
- string
- bool (`true`/`false`)
- `null`
- table (object)
- function

Examples:

```protolex
n = 42
x = 3.14
b = true
s = "proto"
none = null
```

Numeric literal forms:

```protolex
n1 = 42
n2 = 0b101010
n3 = 0x2A
f1 = 3.14
f2 = .5
f3 = 5.
f4 = 1e3
```

Type predicates:

```protolex
isInt(42)       # true
isFloat(3.14)   # true
isString("x")   # true
isBool(false)   # true
isTable([ x = 1 ])
isFunction(fn(x) { x })
```

## 3. Tables (objects) and slots

Tables are created with a literal. Slots are accessed with `.`.

```protolex
user = [
    name = "Ava",
    age = 30
]

name = user.name
age = user.age
```

Protolex is object-oriented in a prototype-based way: objects are tables and
behavior is modeled with explicit functions plus prototypes.

### Prototypes

A table can reference another table as its prototype via the `proto` slot.
Lookups fall back to the prototype chain.

```protolex
base = [ role = "guest" ]
user = [ proto = base, name = "Ava" ]

role = user.role   # "guest" (from proto)
```

This is the inheritance mechanism in Protolex: objects inherit slots from their
`proto` chain.

### clone

`clone(x)` creates a new table whose `proto` is `base`.

```protolex
base = [ x = 1 ]
obj = clone(base)
obj.x   # 1
```

### Local existence vs absence

- `has(obj, key)` checks only local slots on `obj`.
- `isAbsent(expr)` is true when a lookup fails.

```protolex
obj = [ x = 1 ]

has(obj, "x")      # true
has(obj, "y")      # false

isAbsent(obj.y)    # true
isAbsent(obj.x)    # false
```

## 4. Functions and blocks

Functions are values. The last expression in a block is the result.

```protolex
add = fn(a, b) { a + b }

sum = add(2, 3)  # 5
```

### No `return`

Protolex has no `return` statement. The last expression is the result.

```protolex
abs = fn(x) {
    if x < 0 { -x } else { x }
}
```

Instead of early returns, make branches produce the value:

```protolex
scale = fn(x) {
    if x == 0 { 0 } else { x * 2 }
}
```

`if` is an expression:

```protolex
max2 = fn(a, b) {
    if a > b { a } else { b }
}
```

Exceptions use `try / catch / finally`:

```protolex
danger = fn() {
    throw "boom"
}

msg = ""
try {
    danger()
} catch e {
    msg = e
} finally {
    msg = "handled"
}
```

## 5. Mutation

Tables are modified only inside a `mutate` block.

```protolex
obj = [ count = 0 ]

mutate obj {
    obj.count = obj.count + 1
}
```

Removing a slot uses `undefine`:

```protolex
obj = [ x = 1, y = 2 ]

mutate obj {
    undefine obj.y
}
```

## 6. Iteration (no loops)

Protolex has no `for` or `while`. Iteration happens through library functions.
When you need a custom traversal, use explicit recursion.

### Lists

```protolex
import ds from "corelib/ds/index.plx"

list = ds.List.cons(1,
       ds.List.cons(2,
       ds.List.cons(3,
       ds.List.nil)))

mapped = ds.List.map(fn(x) { x * 2 }, list)

sum = ds.List.fold(fn(acc, x) { acc + x }, 0, list)
```

Recursive iteration example:

```protolex
sum_list = fn(lst) {
    if ds.List.isNil(lst) {
        0
    } else {
        ds.List.head(lst) + sum_list(ds.List.tail(lst))
    }
}
```

### Arrays

```protolex
import ds from "corelib/ds/index.plx"

arr = ds.Array.new()

ds.Array.push(arr, 10)
ds.Array.push(arr, 20)

ds.Array.forEach(fn(x) { x }, arr)
```

### Strings

```protolex
import string from "runtime/string"

chars = [ acc = "" ]
string.forEach("hi", fn(c, i) {
    mutate chars { chars.acc = string.concat(chars.acc, c) }
})
```

`string.forEach` iterates bytes (one-byte strings). There is no Unicode-aware
iterator in the runtime today.

String literals support escapes:

```protolex
"\n" "\t" "\r" "\"" "\\"
```

## 7. Core data structures

All structures live in the corelib `ds` module.

### Map

```protolex
import ds from "corelib/ds/index.plx"

m = ds.Map.new()

ds.Map.put(m, "user", "ava")
name = ds.Map.get(m, "user")
exists = ds.Map.has(m, "user")
```

### Set

```protolex
import ds from "corelib/ds/index.plx"

s = ds.Set.new()

ds.Set.add(s, "a")
ds.Set.add(s, "b")

ok = ds.Set.contains(s, "a")
```

### Graph

```protolex
import ds from "corelib/ds/index.plx"

edges = ds.Array.new()

ds.Array.push(edges, [ from = "a", to = "b" ])
ds.Array.push(edges, [ from = "b", to = "c" ])

g = ds.Graph.of(edges)

nbs = ds.Graph.neighbors(g, "b")
```

### Tree

```protolex
import ds from "corelib/ds/index.plx"

compare = fn(a, b) { a < b }

t = ds.Tree.empty()

t = ds.Tree.insert(t, 3, compare)

t = ds.Tree.insert(t, 1, compare)

t = ds.Tree.insert(t, 5, compare)

has5 = ds.Tree.contains(t, 5, compare)
```

## 8. Runtime libraries

Runtime libraries are imported with `runtime/...`.

### string

```protolex
import string from "runtime/string"

len = string.length("proto")
idx = string.indexOf("proto", "to")
rep = string.repeat("ha", 3)
msg = string.format("x=%d", 7)
```

### int / float / math

```protolex
import int from "runtime/int"
import float from "runtime/float"
import math from "runtime/math"

v1 = int.clamp(10, 0, 5)
v2 = float.sqrt(9.0)
pi = math.pi
r = math.random()
```

### io

```protolex
import io from "runtime/io"

f = io.open("notes.txt", "w")
try {
    io.write(f, "hello\n")
} finally {
    io.close(f)
}
```

### sys

```protolex
import sys from "runtime/sys"

args = sys.args
cwd = sys.cwd()
```

### time

```protolex
import time from "runtime/time"

start = time.monotonic()
end = time.monotonic()
```

### log

```protolex
import log from "runtime/log"

log.info("ready")
log.warn("slow")
log.error("boom")
```

## 9. Modules and imports

Each file is a module. Imports are explicit.

```protolex
import ds from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"
import string from "runtime/string"
```

Relative imports work for your own code:

```protolex
import util from "../lib/util.plx"
```

## 10. Small practical examples

### Word count (simple)

```protolex
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

text = "a b a c"
parts = string.split(text, " ")

counts = ds.Map.new()

step = fn(lst) {
    if ds.List.isNil(lst) {
        counts
    } else {
        word = ds.List.head(lst)
        current = ds.Map.get(counts, word)
        next = if isAbsent(current) { 1 } else { current + 1 }
        ds.Map.put(counts, word, next)
        step(ds.List.tail(lst))
    }
}

step(parts)
```

### Filter and sort numbers

```protolex
import ds from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"

arr = ds.Array.new()

ds.Array.push(arr, 5)
ds.Array.push(arr, 2)
ds.Array.push(arr, 9)

ds.Array.push(arr, 1)

less = fn(a, b) { a < b }

sorted = algo.Sort.quick(arr, less)
```

### Simple CLI validation

```protolex
import sys from "runtime/sys"
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

args = sys.args

get0 = fn(lst) {
    if ds.List.isNil(lst) { null } else { ds.List.head(lst) }
}

first = get0(args)

if first == null {
    throw "missing argument"
}

n = string.toInt(first)
```

## 11. Real task examples

### 11.1 CLI tool: file stats (lines/words/chars)

This tool expects a file path and prints basic counts.

```protolex
import sys from "runtime/sys"
import io from "runtime/io"
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

nth = fn(lst, idx) {
    if ds.List.isNil(lst) {
        null
    } else if idx == 0 {
        ds.List.head(lst)
    } else {
        nth(ds.List.tail(lst), idx - 1)
    }
}

path = nth(sys.args, 0)

if path == null {
    throw "usage: filestats <path>"
}

f = io.open(path, "r")
text = ""
try {
    text = io.read(f)
} finally {
    io.close(f)
}

lines = ds.List.length(string.split(text, "\n"))
words = ds.List.length(string.split(text, " "))
chars = string.length(text)

report = string.format("lines=%d words=%d chars=%d\n", lines, words, chars)
io.write(io.stdout, report)
```

### 11.2 File processing: extract error lines

Read a log file and keep only lines that contain `ERROR`.

```protolex
import io from "runtime/io"
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

path = "app.log"

f = io.open(path, "r")
text = ""
try {
    text = io.read(f)
} finally {
    io.close(f)
}

lines = string.split(text, "\n")
errors = ds.List.filter(fn(line) {
    string.indexOf(line, "ERROR") >= 0
}, lines)

out = ds.List.fold(fn(acc, line) {
    string.concat(acc, string.concat(line, "\n"))
}, "", errors)

io.write(io.stdout, out)
```

### 11.3 Report generation: simple daily summary

Build a tiny report from structured data and write it to a file.

```protolex
import io from "runtime/io"
import string from "runtime/string"
import ds from "corelib/ds/index.plx"

items = ds.Array.new()
ds.Array.push(items, [ name = "Emails", count = 12 ])
ds.Array.push(items, [ name = "Builds", count = 3 ])
ds.Array.push(items, [ name = "Tickets", count = 5 ])

line = fn(item) {
    string.format("- %s: %d\n", item.name, item.count)
}

body = ds.Array.fold(fn(acc, item) {
    string.concat(acc, line(item))
}, "", items)

report = string.concat("Daily report\n", body)

f = io.open("daily_report.txt", "w")
try {
    io.write(f, report)
} finally {
    io.close(f)
}
```
