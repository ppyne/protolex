# protolex-corelib — ds/queue
# Version 0.1

import Array from "array.plx"

Queue = [
    proto = null
]

Queue.new = fn() {
    q = [
        proto = Queue,
        _impl = Array.new(),
        head = 0
    ]
    q
}

Queue.enqueue = fn(q, value) {
    Array.push(q._impl, value)
}

Queue.dequeue = fn(q) {
    if q.head >= Array.length(q._impl) {
        throw "Queue.dequeue on empty"
    }
    value = Array.get(q._impl, q.head)
    mutate q {
        q.head = q.head + 1
    }
    value
}

freeze(Queue)

Queue
