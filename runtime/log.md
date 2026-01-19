# runtime/log — Logging

## Description

`log` provides minimal diagnostic output independent of stdout/stderr usage.

## Public interface

```protolex
import log from "runtime/log"

log.info(value)
log.warn(value)
log.error(value)
```

## Semantic rules

- no configurable levels in 0.1
- no complex formatting
- output destination depends on the runtime

## Examples

```protolex
import log from "runtime/log"

log.info("ready")
log.warn("slow")
log.error("boom")
```

## Possible errors

- invalid arguments
