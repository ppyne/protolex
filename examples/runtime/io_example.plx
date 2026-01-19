import io from "runtime/io"

f = io.open("/tmp/protolex_io_example.txt", "w")
try {
    io.write(f, "hello from protolex\n")
} finally {
    io.close(f)
}

f2 = io.open("/tmp/protolex_io_example.txt", "r")
content = null
try {
    content = io.read(f2)
} finally {
    io.close(f2)
}
