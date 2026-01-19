# runtime/sys — System

## Description

`sys` provides minimal access to the execution environment.

## Public interface

```protolex
import sys from "runtime/sys"

sys.args       -> list (immutable)
sys.env        -> table (read-only)
sys.cwd()      -> string
sys.platform() -> string
sys.exit(code) -> never returns
```

## Semantic rules

- `env` is read-only
- no environment mutation
- `exit` terminates immediately
- no shell execution

## Examples

```protolex
import sys from "runtime/sys"

args = sys.args
cwd = sys.cwd()
```

## Possible errors

- invalid arguments
- cwd access failure
