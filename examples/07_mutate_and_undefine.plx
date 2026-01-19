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

vx = obj.x      # 10
vy = obj.y      # undefined (lookup échoue)
