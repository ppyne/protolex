![Protolex](header.png)

# Protolex — Specification 0.1 (consolidated version)

## 0. Language goal

Protolex is a minimalist prototype-based language, with no implicit globals and no semantic magic.
It favors **strong invariants**, **deterministic behavior**, and **systematic explicitness** of effects (mutation, control flow, dependencies).

---

## 1. General ontology

### 1.1. Everything is a value

Protolex defines only values.

Primitive types in 0.1:

- `int` (signed 64-bit integer)

- `float` (double precision)

- `bool`

- `null`

- `string` (immutable)

- `function`

- `table`

There is no conceptual distinction between expression and statement.

### 1.1.1. Numeric literals

Integer literals (`int`) may be written in:

- decimal: `42`, `0`, `-17`

- binary: `0b101010`, `0B101010`

- octal: `0o52`, `0O52`

- hexadecimal: `0x2A`, `0X2A`

Rules:

- prefixes select the base

- there is no implicit octal (`0755` is decimal)

- overflow beyond 64-bit signed range is an error

- all forms produce an `int`

Floating-point literals (`float`) may be written as:

- decimal: `3.14`, `0.5`, `10.0`

- no integer part: `.5`

- no fractional part: `5.`

- scientific notation: `1e3`, `1E3`, `1.5e-4`, `2E+6`

Rules:

- any presence of `.` or `e`/`E` makes the literal a `float`

- no suffixes (`f`, `d`, etc.)

- the value is a double-precision float

- invalid forms are errors

After parsing, there is no trace of the original literal form.

### 1.2. Type predicates

The language provides explicit type predicates:

`isInt(x) isFloat(x) isString(x) isBool(x) isTable(x) isFunction(x)`

Each predicate returns a boolean and never performs coercion.

---

### 1.3. Numeric primitives

- `int` and `float` remain primitive types

- the language provides basic arithmetic operators

- NaN and Infinity are explicitly allowed for `float`

- any advanced numerical operation is provided by runtime libraries

---

### 1.4. Tables

A table is the **only composite type** in the language.

Characteristics:

- associative only

- keys: `string` or `symbol`

- values: any protolex value

- **unordered**

- no iteration order guarantee

- no list or sequence semantics

---

## 2. Objects and prototypes

### 2.1. Object

An object is a table.

### 2.2. Prototype

- the reserved key `proto` designates the prototype

- `proto` may be:

  - an object

  - `null`

- any other value assigned to `proto` is a fatal error

An object without a `proto` key implicitly has `proto = null`.

---

### 2.3. Prototype chain

- the chain is linear

- cycles are **strictly forbidden**

- any attempt to create a cycle causes a fatal, non-interceptable error

---

## 3. Lookup

### 3.1. Syntax

`obj.key`

### 3.2. Semantics

- search in `obj`

- otherwise in `obj.proto`

- recursively until `proto = null`

- the first occurrence found is returned

- no side effects

- no information about the origin of the slot is exposed

### 3.3. Absence

If a key is not found on an object or anywhere in its prototype chain, the lookup results in an absence.

The language provides a standard predicate:

```protolex
    isAbsent(expr)
```

`isAbsent(expr)` returns true if evaluating `expr` resulted in a missing slot,
i.e. if a lookup failed on the target object and its entire prototype chain.

Absence is not a value: it cannot be stored, compared, or passed around.

If `expr` evaluates to a value (including `null`), `isAbsent(expr)` returns false.

If evaluating `expr` throws an exception, it is propagated normally.

---

## 4. Local existence

### 4.1. Function `has`

`has(obj, key)`

- returns `true` if `key` is a **local** slot of `obj`

- never traverses the prototype chain

There is no mechanism to determine from which prototype an inherited slot comes.

---

## 5. Functions

### 5.1. Functions as values

- a function is an ordinary value

- no function is bound to an object

- there is no notion of method at the language level

- no implicit variable (`this`, `self`) is defined

Using a parameter named `self` is a **convention**, not a keyword.

---

### 5.2. Call

`f(x)`

No implicit semantics are attached to calls.

### 5.3. "Method" call convention

`obj.f(obj)`

This form is strictly equivalent to:

`(f = obj.f; f(obj))`

---

## 6. Mutation and freeze

### 6.1. Freeze by default

Every object is **frozen by default**.

Forbidden on a frozen object:

- adding a slot

- modifying a slot

- deleting a slot

Any attempt raises an exception.

---

### 6.2. `mutate` block

`mutate obj {     ... }`

- temporarily allows mutation of `obj`

- thawing is **lexical**

- freeze is automatically restored at block exit

- no permanent mutation is possible

---

### 6.3. Masking and prototypes

- mutating an object never mutates its prototype

- it is allowed to mask an inherited slot with a local slot

- freeze applies only to the targeted object

---

### 6.4. Slot deletion

Slot deletion is done via:

`undefine obj.key`

- `undefine` makes the slot absent

- lookup will fall back to the prototype if present

- `undefine` is a mutation: allowed only in `mutate`

---

## 7. Cloning

### 7.1. Function `clone`

`new = clone(proto)`

- creates an empty table

- sets `new.proto = proto`

- copies no slots

- cloning is always allowed

There is no other construction mechanism.

---

## 8. Runtime environment

### 8.1. No global

Protolex defines **no global object**.

Any value used by a program must come from:

- a lexical binding

- an argument

- an import

- the initial environment provided by the runtime

---

### 8.2. Root objects

- the runtime provides a set of root objects

- these objects are bound lexically

- the initial environment is frozen

- no rebinding is allowed

---

## 9. Import

### 9.1. Principle

`mod = import "module.plx" using env`

- the module is executed in an isolated environment

- the environment is explicitly provided

- the module returns an object

- no global state is implicitly shared

---

### 9.2. Exceptions and import

- exceptions cross module boundaries unchanged

- `import` does not intercept any exception

- isolation must be explicit (`try / catch`)

---

## 10. Exceptions

### 10.1. Principle

- `throw` may raise **any value**

- the language does not distinguish error / signal / event

- exceptions are a general mechanism for control-flow break

---

### 10.2. Syntax

`throw value`

`try {     ... } catch e {     ... } catch * {     ... } finally {     ... }`

---

### 10.3. `catch *`

- intercepts any value raised by `throw`

- does not provide access to the value

- intended for protection and isolation

---

### 10.4. `finally`

- executed in all cases

- **cannot `throw` or `return`**

- any attempt causes a fatal error

- an in-flight exception crosses `finally` intact

---

### 10.5. Fatal errors

Some violations of language invariants are **fatal and non-interceptable**, including:

- prototype cycles

- corruption of the mutation model

- runtime environment inconsistency

- internal runtime errors

---

## 11. Reflection

Protolex forbids full reflection over the object graph.

There is no mechanism to:

- traverse the prototype chain

- inspect the origin of a slot

- modify the lookup strategy

- introspect global program state

---

## 12. Data structures

Protolex defines **no built-in complex data structures**.

- tables are unordered

- no list structure is built into the language

- sequential and algorithmic structures are provided by libraries

Libraries may provide:

- lists

- dynamic arrays

- maps

- sets

- trees

- graphs

- sorting and search algorithms

These structures are implemented as ordinary objects with controlled internal mutation.

---

## 12.1. Iteration

Protolex has **no loop syntax**. All iteration is provided by libraries as
higher-order functions.

Examples include:

- `List.forEach`, `List.map`, `List.fold`, `List.filter`

- `Array.forEach`, `Array.map`, `Array.fold`, `Array.length`

The language provides only blocks and functions; iteration is explicit and
library-defined.

---

## 13. Explicitly absent mechanisms

Protolex does not define:

- classes

- multiple inheritance

- constructors

- destructors

- implicit initialization

- implicit cleanup

- implicit variables (`this`, `self`)

- macros

- metaclasses

- silent monkey-patching

- `for`, `foreach`, `while`

---

## 14. Guiding principles

- no global

- no magic

- no implicit fallback

- strong invariants

- secure by default

- everything is explicit or does not exist

---

## 15. Operators (clarification)

Protolex **provides no operator overloading or redefinition mechanism**,
as in C++ or similar languages.

- Existing syntactic operators are reserved for primitive types.
- They are not extensible by users or libraries.
- Any user-defined operation is expressed **explicitly**
  as a function call.

This choice is intentional and aims to avoid any syntactic magic,
implicit dispatch, and semantic ambiguity.

---

## 16. Status

This specification defines **protolex 0.1**.
It is conceptually closed and sufficient for a complete implementation.

---
