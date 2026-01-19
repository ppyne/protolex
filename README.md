![Protolex](header.png)

# Protolex

***Because explicitness defines existence.***

Protolex is a minimalist, prototype-based language with explicit control over
mutation, control flow, and dependencies. This repository contains:

- the language specification [`specification.md`](specification.md)
- the design rationale [`design.md`](design.md)
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

## Corelib entry points

```plx
import ds   from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"
```

## Repository layout

- [`specification.md`](specification.md) : language spec
- [`design.md`](design.md) : design principles and rationale
- [`corelib.md`](corelib.md) : corelib spec
- [`corelib/ds/`](corelib/ds/) : data structures
- [`corelib/algo/`](corelib/algo/) : algorithms
- [`runtime/`](runtime/) : runtime library docs ([`io.md`](runtime/io.md), [`time.md`](runtime/time.md), [`sys.md`](runtime/sys.md), [`log.md`](runtime/log.md), [`string.md`](runtime/string.md), [`int.md`](runtime/int.md), [`float.md`](runtime/float.md), [`math.md`](runtime/math.md))
- [`examples/`](examples/) : sample programs
- [`src/`](src/) : C interpreter
