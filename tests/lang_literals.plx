assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

assert(0b1010 == 10, "bin")
assert(0o52 == 42, "oct")
assert(0x2A == 42, "hex")
assert(0755 == 755, "no implicit octal")

assert(.5 == 0.5, "leading dot")
assert(5. == 5.0, "trailing dot")
assert(1e3 == 1000.0, "scientific")
assert(1.5e-4 == 0.00015, "scientific fractional")
