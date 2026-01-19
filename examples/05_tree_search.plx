# Exemple 05 — Arbre et recherche

import ds from "corelib/ds.plx"

compare = fn(a, b) { a < b }

t = ds.Tree.empty()

ds.Tree.insert(t, 5, compare)
ds.Tree.insert(t, 2, compare)
ds.Tree.insert(t, 8, compare)

found = ds.Tree.contains(t, 2, compare)
