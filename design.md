![Protolex](header.png)

# Protolex — DESIGN

This document describes the **design principles**, **intentional choices**, and
**mechanisms deliberately absent** from protolex.

It is not a tutorial, but a **justification document** meant to explain *why*
protolex is designed this way, and *why it does not look like* other existing
languages.

---

## 1. Fundamental goal

Protolex aims to provide a language that is:

- minimal
- coherent
- explicit
- without magic
- with strong invariants

The goal is **not** to maximize syntactic concision,
but to **make choices and effects visible** in code.

---

## 2. Central principle: everything is a value

In protolex:

- functions, tables, numbers, booleans, `null` are values
- there is no distinction between expression and statement
- the language introduces no "separate" concept

Direct consequences:
- no implicit lifecycle mechanism
- no hidden phase (init, teardown, etc.)

---

## 3. Objects = tables, explicit prototypes

Protolex is **prototype-based**, but in a strict and minimal way:

- an object is a table
- a prototype is a table referenced by the `proto` key
- lookup is recursive and deterministic
- prototype cycles are forbidden

No other inheritance mechanism exists.

---

## 4. No classes (intentional choice)

Classes are deliberately absent.

Reasons:
- duplicated concepts (class / instance)
- implicit lifecycles
- confusion between structure and behavior
- artificial need for constructors

Protolex prefers:
- explicit cloning
- composition
- prototype sharing

---

## 5. No globals

Protolex defines **no global object**.

Every access must come from:
- a lexical binding
- an argument
- an explicit import
- a root object provided by the runtime

This choice:
- prevents hidden dependencies
- makes isolation easier
- makes modules truly testable

---

## 6. Freeze by default, explicit mutation

Every object is **frozen by default**.

Mutation is:

- always local
- always explicit
- always enclosed by `mutate`

```protolex
mutate obj {
    obj.x = 1
}
```

This model:

- prevents accidental global mutation
- makes effects visible in code
- avoids silent monkey patching

---

## 7. Deletion by absence (`undefine`)

Protolex does not "delete" a value:
it makes a slot **absent**.

```protolex
undefine obj.key
```

Consequences:

- lookup can fall back to the prototype
- no special "deleted" value
- the standard predicate `isAbsent(expr)` can be used to test absence

---

## 8. Functions without implicit binding

Protolex defines:

- no `this`
- no `self`
- no implicit equivalent

A function is an autonomous value.

Passing the object explicitly:

```protolex
obj.f(obj)
```

is considered a **virtue**, not a weakness:

- no ambiguity
- no hidden capture
- no implicit dependency on context

---

## 9. No operator overloading

Protolex **explicitly rejects** any `operator`-style mechanism:

- no overloading
- no redefinition
- no implicit dispatch

Reasons:

- avoid syntactic magic
- avoid resolution ambiguity
- preserve local readability

All operations are **explicit functions**:

```protolex
List.map(f, list)
Sort.quick(array, less)
Map.put(m, k, v)
```

---

## 9.1. Explicit type predicates

Protolex provides explicit predicates for primitive and structural types:

`isInt(x) isFloat(x) isString(x) isBool(x) isTable(x) isFunction(x)`

They are pure, have no side effects, and never coerce values.

---

## 10. Unordered tables

Protolex tables are **fundamentally unordered**.

They must not be used as:

- lists
- arrays
- sequences

This choice:

- avoids implicit order dependencies
- forces explicit structures
- prevents conceptual false friends

---

## 11. No collection literals

Protolex defines no literal for:

- lists
- maps
- sets

Collection construction is performed by the corelib:

```protolex
List.of(listOrArray)
Map.of(pairs)
```

This allows:

- explicit syntax
- no parser extension
- a clear language/library separation

---

## 12. Corelib: ergonomics without magic

The corelib provides:

- classic data structures
- sorting and search algorithms

But:

- no language rule
- no direct user mutation
- no special operator

Internal mutation is **encapsulated** in the library.

---

## 13. Exceptions as a general mechanism

Protolex uses exceptions as a **general control-flow break mechanism**:

- `throw` may raise any value
- `catch *` enables isolation
- `finally` is strict and non-disruptable

This model enables:

- error handling
- event handling
- protection of execution boundaries

---

## 14. Deliberately limited reflection

Protolex forbids full reflection over the object graph.

It is impossible to:

- inspect the prototype chain
- query the origin of a slot
- dynamically modify lookup

This choice:

- protects invariants
- prevents intrusive frameworks
- preserves readability

---

## 15. What protolex does not try to do

Protolex does not try to be:

- a "fast to write" language
- a macro language
- a heavy metaprogramming language
- a universal language

It aims for:

- clarity
- maintainability
- explicit discipline

---

## 16. Positioning

Protolex is suited for:

- medium to large systems
- long-lived codebases
- contexts where readability matters more than concision

It is intentionally demanding.

---

## 17. Iteration is library-defined

Protolex has **no loop syntax**. There is no `for`, no `foreach`, and no `while`.
Iteration is always expressed by higher-order functions provided by libraries.

This is consistent with:

- unordered tables

- data structures living in the corelib

- no control-flow magic

The language provides only blocks and functions.

---

## 18. Conclusion

Protolex rests on a simple idea:

> **What is not explicit does not exist.**

Every design choice aims to make code:

- readable
- analyzable
- controllable

This document is an integral part of the specification.
