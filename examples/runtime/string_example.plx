import string from "runtime/string"

len = string.length("abc")
joined = string.concat("hello", "world")
sub = string.slice("hello", 1, 4)
idx = string.indexOf("hello", "ll")
starts = string.startsWith("hello", "he")
ends = string.endsWith("hello", "lo")
ch = string.charAt("hello", 1)
state = [ acc = "" ]
string.forEach("hi", fn(c) {
    mutate state { state.acc = string.concat(state.acc, c) }
})
string.forEach("hi", fn(_, i) {
    mutate state { state.acc = string.concat(state.acc, string.format("%d", i)) }
})
repeated = string.repeat("ha", 3)
num = string.toInt("42")
flt = string.toFloat("3.5")
msg = string.format("id=%d name=%s", 7, "proto")
