# protolex

Protolex is a minimalist, prototype-based language with explicit control over
mutation, control flow, and dependencies. This repository contains:

- the language specification (`specification.md`)
- the core library specification (`corelib.md`) and implementation (`corelib/`)
- a reference interpreter in C (`src/`)
- example programs (`examples/`)

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

- [`specification.md`](specification.md) : language spec (in french)
- [`corelib.md`](corelib.md) : corelib spec (in french)
- [`corelib/ds/`](corelib/ds/) : data structures
- [`corelib/algo/`](corelib/algo/) : algorithms
- [`examples/`](examples/) : sample programs
- [`src/`](src/) : C interpreter
