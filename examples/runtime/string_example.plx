import string from "runtime/string"

len = string.length("abc")
joined = string.concat("hello", "world")
sub = string.slice("hello", 1, 4)
idx = string.indexOf("hello", "ll")
starts = string.startsWith("hello", "he")
ends = string.endsWith("hello", "lo")
num = string.toInt("42")
flt = string.toFloat("3.5")
msg = string.format("id=%d name=%s", 7, "proto")
