# protolex — Specification 0.1 (consolidated version)

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

---

### 1.2. Tables

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

- if the key is not found anywhere -> `undefined`

- `undefined` is **not** an assignable value

- the language provides a standard predicate `isAbsent(expr)` to test whether
  evaluating `expr` resulted in a missing slot (lookup failed)

- absence is not a value and cannot be stored or compared

- `null` is a valid value distinct from it

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

---

## 14. Guiding principles

- no global

- no magic

- no implicit fallback

- strong invariants

- secure by default

- everything is explicit or does not exist

---

## 15. Status

This specification defines **protolex 0.1**.
It is conceptually closed and sufficient for a complete implementation.

---

## 13. Operators (clarification)

Protolex **provides no operator overloading or redefinition mechanism**,
as in C++ or similar languages.

- Existing syntactic operators are reserved for primitive types.
- They are not extensible by users or libraries.
- Any user-defined operation is expressed **explicitly**
  as a function call.

This choice is intentional and aims to avoid any syntactic magic,
implicit dispatch, and semantic ambiguity.

---
