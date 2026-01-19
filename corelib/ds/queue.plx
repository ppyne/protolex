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
        loop = fn(i) {
            if i < Array.length(items) {
                Queue.enqueue(q, Array.get(items, i))
                loop(i + 1)
            } else {
                q
            }
        }
        loop(0)
    } else if items.proto == List {
        q = Queue.new()
        loop = fn(lst) {
            if List.isNil(lst) {
                q
            } else {
                Queue.enqueue(q, List.head(lst))
                loop(List.tail(lst))
            }
        }
        loop(items)
    } else {
        throw "Queue.of expects Array or List"
    }
}

freeze(Queue)

Queue
