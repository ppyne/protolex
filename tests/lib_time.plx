import time from "runtime/time"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

start = time.monotonic()
time.sleep(1)
end = time.monotonic()

assert(end >= start, "monotonic")
