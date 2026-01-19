import string from "runtime/string"

assert = fn(cond, msg) {
    if !cond {
        throw msg
    }
}

assert(string.length("abc") == 3, "length")
assert(string.concat("a", "b") == "ab", "concat")
assert(string.slice("hello", 1, 4) == "ell", "slice")
assert(string.charAt("hello", 1) == "e", "charAt")
assert(string.repeat("ha", 3) == "hahaha", "repeat")
state = [ count = 0 ]
string.forEach("abc", fn(c, i) {
    mutate state { state.count = state.count + 1 }
})
assert(state.count == 3, "forEach")
assert(string.indexOf("hello", "ll") == 2, "indexOf")
assert(string.startsWith("hello", "he") == true, "startsWith")
assert(string.endsWith("hello", "lo") == true, "endsWith")
assert(string.toInt("42") == 42, "toInt")
assert(string.toFloat("3.5") == 3.5, "toFloat")
assert(string.format("x=%d y=%s", 3, "ok") == "x=3 y=ok", "format")
