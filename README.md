![Protolex](header.png)

# Protolex

Protolex is a minimalist, prototype-based language with explicit control over
mutation, control flow, and dependencies. This repository contains:

- the language specification (`specification.md`)
- the design rationale (`design.md`)
- the core library specification (`corelib.md`) and implementation (`corelib/`)
- the runtime libraries (`runtime/`), including `io`, `time`, `sys`, `log`, `string`, `int`, `float`, `math`
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

- `specification.md` : language spec
- `design.md` : design principles and rationale
- `corelib.md` : corelib spec
- `corelib/ds/` : data structures
- `corelib/algo/` : algorithms
- `runtime/` : runtime library docs (`io`, `time`, `sys`, `log`, `string`, `int`, `float`, `math`)
- `examples/` : sample programs
- `src/` : C interpreter
