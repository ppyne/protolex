# protolex-corelib — ds/list
# Version 0.1

List = [
    proto = null
]

List.nil = [
    proto = List,
    isNil = true
]

List.cons = fn(head, tail) {
    node = [
        proto = List,
        head = head,
        tail = tail,
        isNil = false
    ]
    node
}

List.isNil = fn(list) {
    list.isNil == true
}

List.head = fn(list) {
    if list.isNil {
        throw "List.head on nil"
    }
    list.head
}

List.tail = fn(list) {
    if list.isNil {
        throw "List.tail on nil"
    }
    list.tail
}

List.length = fn(list) {
    loop = fn(lst, acc) {
        if lst.isNil {
            acc
        } else {
            loop(lst.tail, acc + 1)
        }
    }
    loop(list, 0)
}

List.map = fn(f, list) {
    if list.isNil {
        List.nil
    } else {
        List.cons(
            f(list.head),
            List.map(f, list.tail)
        )
    }
}

List.fold = fn(f, acc, list) {
    if list.isNil {
        acc
    } else {
        List.fold(
            f,
            f(acc, list.head),
            list.tail
        )
    }
}

List.forEach = fn(f, list) {
    if list.isNil {
        null
    } else {
        f(list.head)
        List.forEach(f, list.tail)
    }
}

List.filter = fn(pred, list) {
    if list.isNil {
        List.nil
    } else {
        h = list.head
        t = List.filter(pred, list.tail)
        if pred(h) {
            List.cons(h, t)
        } else {
            t
        }
    }
}

List.reverse = fn(list) {
    loop = fn(lst, acc) {
        if lst.isNil {
            acc
        } else {
            loop(lst.tail, List.cons(lst.head, acc))
        }
    }
    loop(list, List.nil)
}

freeze(List)
freeze(List.nil)

List
