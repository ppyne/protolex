# Exemple 02 — Array + tri explicite

import ds   from "corelib/ds/index.plx"
import algo from "corelib/algo/index.plx"

a = ds.Array.new()

ds.Array.push(a, 3)
ds.Array.push(a, 1)
ds.Array.push(a, 4)
ds.Array.push(a, 2)

algo.Sort.quick(
    a,
    fn(x, y) { x < y }
)

first = ds.Array.get(a, 0)
last  = ds.Array.get(a, ds.Array.length(a) - 1)
