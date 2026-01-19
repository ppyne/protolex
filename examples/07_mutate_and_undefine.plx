# Exemple 07 — mutate et undefine

proto = [
    x = 1,
    y = 2
]

obj = clone(proto)

mutate obj {
    obj.x = 10
    undefine obj.y
}

vx = obj.x
fallback = obj.y          # 2 (lookup falls back to proto)

obj2 = [ proto = null ]
absent = isAbsent(obj2.y) # true (lookup failed)
