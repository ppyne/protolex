import sys from "runtime/sys"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

args = sys.args
assert(args.isNil == false, "args missing")
assert(args.tail.isNil == false, "args tail missing")
assert(args.tail.head == "extra", "args value")

cwd = sys.cwd()
assert(cwd != null, "cwd")
