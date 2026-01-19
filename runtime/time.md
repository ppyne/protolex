# runtime/time — Time

## Description

`time` provides minimal and reliable access to time as raw timestamps.

## Public interface

```protolex
import time from "runtime/time"

time.now()       -> int  # milliseconds since epoch
time.monotonic() -> int  # milliseconds, monotonic
time.sleep(ms)   -> void # blocking
```

## Semantic rules

- no date/calendar utilities
- no formatting
- `sleep` is blocking and explicit

## Examples

```protolex
import time from "runtime/time"

start = time.monotonic()
time.sleep(100)
end = time.monotonic()
```

## Possible errors

- invalid arguments
- clock failures
