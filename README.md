![Protolex](header.png)

# Protolex

***Because explicitness defines existence.***

Protolex is a minimalist, prototype-based language with explicit control over
mutation, control flow, and dependencies. This repository contains:

Key constraints: no loops, no `return`, no globals; iteration is library-defined.

- the language specification [`specification.md`](specification.md)
- the design rationale [`design.md`](design.md)
- the user manual [`MANUAL.md`](MANUAL.md)
- the newcomer guide [`FROM_C_PY_PHP_JS.md`](FROM_C_PY_PHP_JS.md)
- the core library specification [`corelib.md`](corelib.md) and implementation [`corelib/`](corelib/)
- the runtime libraries [`runtime/`](runtime/), including [`io`](runtime/io), [`time`](runtime/time.md), [`sys`](runtime/sys.md), [`log`](runtime/log.md), [`string`](runtime/string.md), [`int`](runtime/int.md), [`float`](runtime/float.md), [`math`](runtime/math.md)
- a reference interpreter in C [`src/`](src/)
- example programs [`examples/`](examples/)

## Quick start

Build the interpreter:

```sh
cd src
make
```

Run an example:

```sh
./protolex ../examples/02_array_and_sort.plx
```

Next:

- read the manual [`MANUAL.md`](MANUAL.md)
- read the guide [`FROM_C_PY_PHP_JS.md`](FROM_C_PY_PHP_JS.md)

## Corelib entry points

```plx
import ds   from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"
```

## Repository layout

- [`specification.md`](specification.md) : language spec
- [`design.md`](design.md) : design principles and rationale
- [`MANUAL.md`](MANUAL.md) : user manual
- [`FROM_C_PY_PHP_JS.md`](FROM_C_PY_PHP_JS.md) : guide for C/Python/PHP/JS developers
- [`corelib.md`](corelib.md) : corelib spec
- [`corelib/ds/`](corelib/ds/) : data structures
- [`corelib/algo/`](corelib/algo/) : algorithms
- [`runtime/`](runtime/) : runtime library docs ([`io.md`](runtime/io.md), [`time.md`](runtime/time.md), [`sys.md`](runtime/sys.md), [`log.md`](runtime/log.md), [`string.md`](runtime/string.md), [`int.md`](runtime/int.md), [`float.md`](runtime/float.md), [`math.md`](runtime/math.md))
- [`examples/`](examples/) : sample programs
- [`src/`](src/) : C interpreter
