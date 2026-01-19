import io from "runtime/io"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

path = "/tmp/protolex_test_io.txt"

f = io.open(path, "w")
try {
    io.write(f, "hello")
} finally {
    io.close(f)
}

f2 = io.open(path, "r")
content = null
try {
    content = io.read(f2)
} finally {
    io.close(f2)
}

assert(content == "hello", "io content")
