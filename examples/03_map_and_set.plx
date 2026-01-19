# Exemple 03 — Map et Set

import ds from "corelib/ds.plx"

m = ds.Map.new()

ds.Map.put(m, "a", 1)
ds.Map.put(m, "b", 2)

va = ds.Map.get(m, "a")
hb = ds.Map.has(m, "b")

s = ds.Set.new()

ds.Set.add(s, "apple")
ds.Set.add(s, "banana")

has_apple = ds.Set.contains(s, "apple")
