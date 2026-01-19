#include <stdlib.h>
#include <string.h>

#include "runtime_string.h"

static Table *make_list_nil(void) {
    Table *nil = table_new();
    table_set(nil, make_string_value("isNil", 5), make_bool(true));
    table_freeze(nil);
    return nil;
}

static Table *make_list_cons(Value head, Table *tail) {
    Table *node = table_new();
    table_set(node, make_string_value("isNil", 5), make_bool(false));
    table_set(node, make_string_value("head", 4), head);
    table_set(node, make_string_value("tail", 4), make_table(tail));
    table_freeze(node);
    return node;
}

static Value native_string_length(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "string.length expects (string)");
        return make_null();
    }
    return make_int((int64_t)argv[0].as.str->len);
}

static Value native_string_concat(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "string.concat expects (string, string)");
        return make_null();
    }
    size_t len = argv[0].as.str->len + argv[1].as.str->len;
    char *buf = xmalloc(len);
    memcpy(buf, argv[0].as.str->data, argv[0].as.str->len);
    memcpy(buf + argv[0].as.str->len, argv[1].as.str->data, argv[1].as.str->len);
    Value out = make_string_value(buf, len);
    free(buf);
    return out;
}

static Value native_string_slice(int argc, Value *argv, EvalResult *err) {
    if (argc != 3 || argv[0].type != VAL_STRING ||
        argv[1].type != VAL_INT || argv[2].type != VAL_INT) {
        runtime_set_error(err, "string.slice expects (string, int, int)");
        return make_null();
    }
    int64_t start = argv[1].as.i;
    int64_t end = argv[2].as.i;
    if (start < 0 || end < 0 || start > end) {
        runtime_set_error(err, "string.slice invalid range");
        return make_null();
    }
    size_t len = argv[0].as.str->len;
    if ((size_t)start > len) {
        start = (int64_t)len;
    }
    if ((size_t)end > len) {
        end = (int64_t)len;
    }
    size_t out_len = (size_t)(end - start);
    return make_string_value(argv[0].as.str->data + start, out_len);
}

static Value native_string_index_of(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "string.indexOf expects (string, string)");
        return make_null();
    }
    if (argv[1].as.str->len == 0) {
        return make_int(0);
    }
    if (argv[1].as.str->len > argv[0].as.str->len) {
        return make_int(-1);
    }
    const char *hay = argv[0].as.str->data;
    const char *needle = argv[1].as.str->data;
    size_t hlen = argv[0].as.str->len;
    size_t nlen = argv[1].as.str->len;
    for (size_t i = 0; i + nlen <= hlen; i++) {
        if (memcmp(hay + i, needle, nlen) == 0) {
            return make_int((int64_t)i);
        }
    }
    return make_int(-1);
}

static Value native_string_starts_with(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "string.startsWith expects (string, string)");
        return make_null();
    }
    if (argv[1].as.str->len > argv[0].as.str->len) {
        return make_bool(false);
    }
    return make_bool(memcmp(argv[0].as.str->data, argv[1].as.str->data,
                            argv[1].as.str->len) == 0);
}

static Value native_string_ends_with(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "string.endsWith expects (string, string)");
        return make_null();
    }
    if (argv[1].as.str->len > argv[0].as.str->len) {
        return make_bool(false);
    }
    size_t offset = argv[0].as.str->len - argv[1].as.str->len;
    return make_bool(memcmp(argv[0].as.str->data + offset, argv[1].as.str->data,
                            argv[1].as.str->len) == 0);
}

static Value native_string_split(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "string.split expects (string, string)");
        return make_null();
    }
    if (argv[1].as.str->len == 0) {
        runtime_set_error(err, "string.split empty separator");
        return make_null();
    }
    const char *s = argv[0].as.str->data;
    size_t len = argv[0].as.str->len;
    const char *sep = argv[1].as.str->data;
    size_t sep_len = argv[1].as.str->len;

    Value *parts = NULL;
    size_t count = 0;
    size_t cap = 0;

    size_t i = 0;
    size_t start = 0;
    while (i + sep_len <= len) {
        if (memcmp(s + i, sep, sep_len) == 0) {
            Value part = make_string_value(s + start, i - start);
            if (count == cap) {
                size_t new_cap = cap ? cap * 2 : 8;
                parts = realloc(parts, new_cap * sizeof(Value));
                if (!parts) {
                    runtime_fatal("out of memory");
                }
                cap = new_cap;
            }
            parts[count++] = part;
            i += sep_len;
            start = i;
        } else {
            i++;
        }
    }
    Value part = make_string_value(s + start, len - start);
    if (count == cap) {
        size_t new_cap = cap ? cap * 2 : 8;
        parts = realloc(parts, new_cap * sizeof(Value));
        if (!parts) {
            runtime_fatal("out of memory");
        }
        cap = new_cap;
    }
    parts[count++] = part;

    Table *list = make_list_nil();
    for (size_t idx = count; idx > 0; idx--) {
        list = make_list_cons(parts[idx - 1], list);
    }
    free(parts);
    return make_table(list);
}

static Value native_string_to_int(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "string.toInt expects (string)");
        return make_null();
    }
    char *end = NULL;
    long long val = strtoll(argv[0].as.str->data, &end, 10);
    if (!end || (size_t)(end - argv[0].as.str->data) != argv[0].as.str->len) {
        runtime_set_error(err, "string.toInt invalid");
        return make_null();
    }
    return make_int((int64_t)val);
}

static Value native_string_to_float(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "string.toFloat expects (string)");
        return make_null();
    }
    char *end = NULL;
    double val = strtod(argv[0].as.str->data, &end);
    if (!end || (size_t)(end - argv[0].as.str->data) != argv[0].as.str->len) {
        runtime_set_error(err, "string.toFloat invalid");
        return make_null();
    }
    return make_float(val);
}

Table *runtime_string_build(void) {
    if (runtime_ctx.string) {
        return runtime_ctx.string;
    }
    Table *string = table_new();

    Function *len_fn = xmalloc(sizeof(Function));
    len_fn->is_native = true;
    len_fn->native = native_string_length;
    table_set(string, make_string_value("length", 6), make_function(len_fn));

    Function *concat_fn = xmalloc(sizeof(Function));
    concat_fn->is_native = true;
    concat_fn->native = native_string_concat;
    table_set(string, make_string_value("concat", 6), make_function(concat_fn));

    Function *slice_fn = xmalloc(sizeof(Function));
    slice_fn->is_native = true;
    slice_fn->native = native_string_slice;
    table_set(string, make_string_value("slice", 5), make_function(slice_fn));

    Function *split_fn = xmalloc(sizeof(Function));
    split_fn->is_native = true;
    split_fn->native = native_string_split;
    table_set(string, make_string_value("split", 5), make_function(split_fn));

    Function *index_fn = xmalloc(sizeof(Function));
    index_fn->is_native = true;
    index_fn->native = native_string_index_of;
    table_set(string, make_string_value("indexOf", 7), make_function(index_fn));

    Function *starts_fn = xmalloc(sizeof(Function));
    starts_fn->is_native = true;
    starts_fn->native = native_string_starts_with;
    table_set(string, make_string_value("startsWith", 10), make_function(starts_fn));

    Function *ends_fn = xmalloc(sizeof(Function));
    ends_fn->is_native = true;
    ends_fn->native = native_string_ends_with;
    table_set(string, make_string_value("endsWith", 8), make_function(ends_fn));

    Function *to_int_fn = xmalloc(sizeof(Function));
    to_int_fn->is_native = true;
    to_int_fn->native = native_string_to_int;
    table_set(string, make_string_value("toInt", 5), make_function(to_int_fn));

    Function *to_float_fn = xmalloc(sizeof(Function));
    to_float_fn->is_native = true;
    to_float_fn->native = native_string_to_float;
    table_set(string, make_string_value("toFloat", 7), make_function(to_float_fn));

    table_freeze(string);
    runtime_ctx.string = string;
    return string;
}
