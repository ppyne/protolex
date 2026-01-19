# Exemple 04 — Stack et Queue

import ds from "corelib/ds.plx"

stack = ds.Stack.new()
ds.Stack.push(stack, 10)
ds.Stack.push(stack, 20)

top = ds.Stack.pop(stack)

queue = ds.Queue.new()
ds.Queue.enqueue(queue, "a")
ds.Queue.enqueue(queue, "b")

first = ds.Queue.dequeue(queue)
