# Exemple 08 — Exceptions et finally strict

danger = fn() {
    throw "boom"
}

try {
    danger()
} catch e {
    handled = e
} finally {
    cleanup = true
}
