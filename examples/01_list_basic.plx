# Exemple 01 — Listes immuables

import ds from "corelib/ds/index.plx"

list =
    ds.List.cons(1,
    ds.List.cons(2,
    ds.List.cons(3,
    ds.List.nil)))

length = ds.List.length(list)

mapped =
    ds.List.map(fn(x) { x * 2 }, list)
