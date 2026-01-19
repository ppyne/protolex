# runtime/io — I/O

## Description

`io` provides explicit file and stream I/O for protolex.
All operations are synchronous and must be invoked explicitly.

## Public interface

```protolex
import io from "runtime/io"

io.open(path, mode)      -> file
io.read(file)            -> string
io.write(file, data)     -> void
io.close(file)           -> void

io.stdin
io.stdout
io.stderr
```

## Semantic rules

- no auto-close
- cleanup must be explicit (use `finally`)
- any error throws
- `file` is an opaque object; user mutation is not allowed

## Examples

```protolex
import io from "runtime/io"

f = io.open("notes.txt", "w")
try {
    io.write(f, "hello\n")
} finally {
    io.close(f)
}
```

## Possible errors

- invalid arguments
- open/read/write failures
- closing an invalid file
