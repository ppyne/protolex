# protolex-corelib — ds/queue
# Version 0.1

import Array from "array.plx"
import List from "list.plx"

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

Queue.of = fn(items) {
    if items.proto == Queue {
        items
    } else if items.proto == Array {
        q = Queue.new()
        i = 0
        n = Array.length(items)
        while i < n {
            Queue.enqueue(q, Array.get(items, i))
            i = i + 1
        }
        q
    } else if items.proto == List {
        q = Queue.new()
        cur = items
        while !List.isNil(cur) {
            Queue.enqueue(q, List.head(cur))
            cur = List.tail(cur)
        }
        q
    } else {
        throw "Queue.of expects Array or List"
    }
}

freeze(Queue)

Queue
