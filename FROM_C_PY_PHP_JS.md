![Protolex](header.png)

# Protolex for C, C++, Python, PHP, and JavaScript Developers

This guide shows what changes when you move to Protolex.
It focuses on concrete differences and working patterns.

## 1. What is familiar

- functions are values
- data is passed explicitly
- `if` is an expression
- `try / catch / finally` exist
- strings and numbers are primitive values

## 2. The big differences (all languages)

### No loops

There is no `for` / `while` / `foreach`.
Iteration is always done by library functions, or by writing explicit recursion.

```protolex
import ds from "corelib/ds/index.plx"

arr = ds.Array.new()

ds.Array.push(arr, 1)
ds.Array.push(arr, 2)

ds.Array.forEach(fn(x) { x }, arr)
```

Recursive iteration example:

```protolex
import ds from "corelib/ds/index.plx"

sum_list = fn(lst) {
    if ds.List.isNil(lst) {
        0
    } else {
        ds.List.head(lst) + sum_list(ds.List.tail(lst))
    }
}
```

### How to replace `while`, `do/while`, `for`, `break`, `continue`

Protolex has no loop syntax, so you model loops with recursion and explicit
control values. Here are direct equivalents.

**`while (cond) { body }`**

```protolex
loop = fn(state) {
    if cond(state) {
        loop(step(state))
    } else {
        state
    }
}
```

**`do { body } while (cond);`**

```protolex
loop = fn(state) {
    next = step(state)
    if cond(next) { loop(next) } else { next }
}
```

**`for (i = 0; i < len; i++)`**

```protolex
loop = fn(i, len, state) {
    if i < len {
        loop(i + 1, len, step(i, state))
    } else {
        state
    }
}
```

**`continue` (skip to next iteration)**

```protolex
loop = fn(i, len, state) {
    if i < len {
        if shouldSkip(i) {
            loop(i + 1, len, state)
        } else {
            loop(i + 1, len, step(i, state))
        }
    } else {
        state
    }
}
```

**`break` (early exit)**

Use an explicit stop condition or raise an exception that the iterator
captures.

```protolex
Stop = "StopIteration"

loop = fn(i, len, state) {
    if i < len {
        if shouldStop(i) { throw Stop } else { loop(i + 1, len, step(i, state)) }
    } else {
        state
    }
}

result = try { loop(0, len, init) } catch e { init }
```

### No `return`

The last expression is the function result.

```protolex
sum = fn(a, b) { a + b }

abs = fn(x) {
    if x < 0 { -x } else { x }
}
```

### No globals

There is no global state. Everything is local or imported.

```protolex
import string from "runtime/string"

greet = fn(name) { string.concat("hi ", name) }
```

### Tables instead of classes

Protolex has tables and prototypes, not classes or methods.

```protolex
point = [ x = 3, y = 4 ]

distance2 = fn(p) { p.x * p.x + p.y * p.y }
```

### Absence vs `null`

Missing slots are *absent*. Absence is not a value.
Use `isAbsent(expr)` to test lookup failure.

```protolex
obj = [ x = 1 ]

isAbsent(obj.y)  # true
obj.x == null    # false
```

### Explicit mutation

Tables are frozen by default. Mutation requires `mutate`.

```protolex
state = [ count = 0 ]

mutate state {
    state.count = state.count + 1
}
```

## 3. From C

### No manual memory, no pointers

Protolex values are managed by the runtime. You never malloc/free.

### No headers, no globals

Modules are explicit imports:

```protolex
import ds from "corelib/ds/index.plx"
```

### Function result is the last expression

```protolex
max2 = fn(a, b) { if a > b { a } else { b } }
```

## 4. From C++

### No classes, constructors, or methods

Protolex is object-oriented in a prototype-based way. It uses tables and
explicit functions, not classes or methods.

```protolex
point = [ x = 3, y = 4 ]
norm2 = fn(p) { p.x * p.x + p.y * p.y }
```

### No RAII or destructors

Resources must be closed explicitly with `try/finally`.

```protolex
import io from "runtime/io"

f = io.open("notes.txt", "w")
try {
    io.write(f, "hello\n")
} finally {
    io.close(f)
}
```

### No operator overloading

All operations beyond primitives are explicit function calls.

```protolex
import ds from "corelib/ds/index.plx"

ds.List.map(fn(x) { x * 2 }, list)
```

## 5. From Python

### No implicit iteration

Use `List.map` / `Array.forEach` instead of `for`.

```protolex
import ds from "corelib/ds/index.plx"

list = ds.List.cons(1, ds.List.cons(2, ds.List.nil))

doubled = ds.List.map(fn(x) { x * 2 }, list)
```

### Be explicit about emptiness

Do not rely on container truthiness. Check explicitly.

```protolex
if ds.List.isNil(list) { "empty" } else { "not empty" }
```

## 6. From PHP

### No associative arrays as objects

Protolex is object-oriented in a prototype-based way. Use `Map` for key/value
storage, and tables for structured data.

```protolex
import ds from "corelib/ds/index.plx"

m = ds.Map.new()

ds.Map.put(m, "user", "ava")
name = ds.Map.get(m, "user")
```

### No automatic string/number coercion

Conversions are explicit:

```protolex
import string from "runtime/string"

n = string.toInt("42")
```

## 7. From JavaScript

### No methods or `this`

Functions do not bind to objects. Pass the object explicitly.

```protolex
import ds from "corelib/ds/index.plx"

size = fn(arr) { ds.Array.length(arr) }
```

### No implicit prototype mutation

Prototypes are explicit via the `proto` slot.

```protolex
base = [ role = "guest" ]
user = [ proto = base, name = "Ava" ]
```

### No implicit iteration over objects

Tables are unordered. Iteration is provided by libraries only.

## 8. Everyday tasks (short examples)

### Read a file and print it

```protolex
import io from "runtime/io"

f = io.open("notes.txt", "r")
text = ""
try {
    text = io.read(f)
} finally {
    io.close(f)
}

io.write(io.stdout, text)
```

### Count words (simple)

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

## 9. Quick reference

- No loops: use `List.*` / `Array.*` / `string.forEach`
- No `return`: last expression is the result
- No globals: use explicit imports and parameters
- Tables are frozen: use `mutate` to change
- Missing slots are absence: use `isAbsent(expr)`
