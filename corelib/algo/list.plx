# protolex-corelib — algo/list
# Version 0.1

import ds from "../ds/index.plx"

ListAlgo = [
    proto = null
]

ListAlgo.isEmpty = fn(list) {
    ds.List.isNil(list)
}

ListAlgo.length = fn(list) {
    ds.List.length(list)
}

ListAlgo.map = fn(f, list) {
    ds.List.map(f, list)
}

ListAlgo.filter = fn(pred, list) {
    if ds.List.isNil(list) {
        ds.List.nil
    } else {
        h = ds.List.head(list)
        t = ds.List.tail(list)
        if pred(h) {
            ds.List.cons(h, ListAlgo.filter(pred, t))
        } else {
            ListAlgo.filter(pred, t)
        }
    }
}

ListAlgo.foldLeft = fn(f, acc, list) {
    if ds.List.isNil(list) {
        acc
    } else {
        ListAlgo.foldLeft(
            f,
            f(acc, ds.List.head(list)),
            ds.List.tail(list)
        )
    }
}

ListAlgo.foldRight = fn(f, acc, list) {
    if ds.List.isNil(list) {
        acc
    } else {
        f(
            ds.List.head(list),
            ListAlgo.foldRight(f, acc, ds.List.tail(list))
        )
    }
}

ListAlgo.reverse = fn(list) {
    ds.List.reverse(list)
}

ListAlgo.find = fn(pred, list) {
    if ds.List.isNil(list) {
        null
    } else {
        h = ds.List.head(list)
        if pred(h) {
            h
        } else {
            ListAlgo.find(pred, ds.List.tail(list))
        }
    }
}

ListAlgo.contains = fn(value, list) {
    ListAlgo.find(fn(x) { x == value }, list) != null
}

ListAlgo.partition = fn(pred, list) {
    split = fn(lst, yes, no) {
        if ds.List.isNil(lst) {
            [
                yes = ds.List.reverse(yes),
                no  = ds.List.reverse(no)
            ]
        } else {
            h = ds.List.head(lst)
            t = ds.List.tail(lst)
            if pred(h) {
                split(t, ds.List.cons(h, yes), no)
            } else {
                split(t, yes, ds.List.cons(h, no))
            }
        }
    }
    split(list, ds.List.nil, ds.List.nil)
}

ListAlgo.split = fn(list) {
    if ds.List.isNil(list) || ds.List.isNil(ds.List.tail(list)) {
        [
            left  = list,
            right = ds.List.nil
        ]
    } else {
        slow = list
        fast = ds.List.tail(list)

        advance = fn(s, f) {
            if ds.List.isNil(f) || ds.List.isNil(ds.List.tail(f)) {
                s
            } else {
                advance(ds.List.tail(s), ds.List.tail(ds.List.tail(f)))
            }
        }

        mid = advance(slow, fast)
        right = ds.List.tail(mid)

        [
            left  = list,
            right = right,
            cut   = mid
        ]
    }
}

ListAlgo.merge = fn(a, b, less) {
    if ds.List.isNil(a) {
        b
    } else if ds.List.isNil(b) {
        a
    } else {
        ha = ds.List.head(a)
        hb = ds.List.head(b)
        if less(ha, hb) {
            ds.List.cons(ha, ListAlgo.merge(ds.List.tail(a), b, less))
        } else {
            ds.List.cons(hb, ListAlgo.merge(a, ds.List.tail(b), less))
        }
    }
}

ListAlgo.sort = fn(list, less) {
    if ds.List.isNil(list) || ds.List.isNil(ds.List.tail(list)) {
        list
    } else {
        parts = ListAlgo.partition(
            fn(_) { true },
            list
        )
        slow = list
        fast = ds.List.tail(list)

        advance = fn(s, f) {
            if ds.List.isNil(f) || ds.List.isNil(ds.List.tail(f)) {
                s
            } else {
                advance(ds.List.tail(s), ds.List.tail(ds.List.tail(f)))
            }
        }

        mid = advance(slow, fast)
        right = ds.List.tail(mid)

        leftSorted  = ListAlgo.sort(list, less)
        rightSorted = ListAlgo.sort(right, less)

        ListAlgo.merge(leftSorted, rightSorted, less)
    }
}

freeze(ListAlgo)

algo_list = [
    List = ListAlgo
]

algo_list
