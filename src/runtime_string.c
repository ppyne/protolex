#include <stdio.h>
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

static void buffer_append(char **buf, size_t *len, size_t *cap, const char *data, size_t data_len) {
    if (*cap < *len + data_len + 1) {
        size_t new_cap = *cap ? *cap * 2 : 64;
        while (new_cap < *len + data_len + 1) {
            new_cap *= 2;
        }
        char *next = realloc(*buf, new_cap);
        if (!next) {
            runtime_fatal("out of memory");
        }
        *buf = next;
        *cap = new_cap;
    }
    memcpy(*buf + *len, data, data_len);
    *len += data_len;
    (*buf)[*len] = '\0';
}

static Value native_string_format(int argc, Value *argv, EvalResult *err) {
    if (argc < 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "string.format expects (string, ...)");
        return make_null();
    }
    const char *fmt = argv[0].as.str->data;
    size_t fmt_len = argv[0].as.str->len;
    size_t argi = 1;
    char *out = NULL;
    size_t out_len = 0;
    size_t out_cap = 0;

    size_t i = 0;
    while (i < fmt_len) {
        if (fmt[i] != '%') {
            buffer_append(&out, &out_len, &out_cap, &fmt[i], 1);
            i++;
            continue;
        }
        if (i + 1 < fmt_len && fmt[i + 1] == '%') {
            buffer_append(&out, &out_len, &out_cap, "%", 1);
            i += 2;
            continue;
        }
        size_t spec_start = i;
        i++;
        while (i < fmt_len && strchr("+- #0", fmt[i])) {
            i++;
        }
        while (i < fmt_len && (fmt[i] >= '0' && fmt[i] <= '9')) {
            i++;
        }
        if (i < fmt_len && fmt[i] == '.') {
            i++;
            while (i < fmt_len && (fmt[i] >= '0' && fmt[i] <= '9')) {
                i++;
            }
        }
        if (i < fmt_len && (fmt[i] == 'h' || fmt[i] == 'l' || fmt[i] == 'z' ||
                            fmt[i] == 'j' || fmt[i] == 't')) {
            i++;
            if (i < fmt_len && (fmt[i - 1] == 'h' && fmt[i] == 'h')) {
                i++;
            } else if (i < fmt_len && (fmt[i - 1] == 'l' && fmt[i] == 'l')) {
                i++;
            }
        }
        if (i >= fmt_len) {
            runtime_set_error(err, "string.format invalid format");
            free(out);
            return make_null();
        }
        char conv = fmt[i++];
        if (argi >= (size_t)argc) {
            runtime_set_error(err, "string.format missing argument");
            free(out);
            return make_null();
        }
        Value v = argv[argi++];
        char spec[64];
        size_t spec_len = i - spec_start;
        if (spec_len >= sizeof(spec)) {
            runtime_set_error(err, "string.format spec too long");
            free(out);
            return make_null();
        }
        memcpy(spec, fmt + spec_start, spec_len);
        spec[spec_len] = '\0';

        int needed = 0;
        char *tmp = NULL;
        switch (conv) {
        case 'd':
        case 'i':
            if (v.type != VAL_INT) {
                runtime_set_error(err, "string.format expected int");
                free(out);
                return make_null();
            }
            needed = snprintf(NULL, 0, spec, (long long)v.as.i);
            tmp = xmalloc((size_t)needed + 1);
            snprintf(tmp, (size_t)needed + 1, spec, (long long)v.as.i);
            break;
        case 'u':
        case 'x':
        case 'X':
        case 'o':
            if (v.type != VAL_INT) {
                runtime_set_error(err, "string.format expected int");
                free(out);
                return make_null();
            }
            needed = snprintf(NULL, 0, spec, (unsigned long long)v.as.i);
            tmp = xmalloc((size_t)needed + 1);
            snprintf(tmp, (size_t)needed + 1, spec, (unsigned long long)v.as.i);
            break;
        case 'c':
            if (v.type != VAL_INT) {
                runtime_set_error(err, "string.format expected int");
                free(out);
                return make_null();
            }
            needed = snprintf(NULL, 0, spec, (int)v.as.i);
            tmp = xmalloc((size_t)needed + 1);
            snprintf(tmp, (size_t)needed + 1, spec, (int)v.as.i);
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            if (v.type != VAL_FLOAT) {
                runtime_set_error(err, "string.format expected float");
                free(out);
                return make_null();
            }
            needed = snprintf(NULL, 0, spec, v.as.f);
            tmp = xmalloc((size_t)needed + 1);
            snprintf(tmp, (size_t)needed + 1, spec, v.as.f);
            break;
        case 's':
            if (v.type != VAL_STRING) {
                runtime_set_error(err, "string.format expected string");
                free(out);
                return make_null();
            }
            needed = snprintf(NULL, 0, spec, v.as.str->data);
            tmp = xmalloc((size_t)needed + 1);
            snprintf(tmp, (size_t)needed + 1, spec, v.as.str->data);
            break;
        default:
            runtime_set_error(err, "string.format unsupported spec");
            free(out);
            return make_null();
        }

        buffer_append(&out, &out_len, &out_cap, tmp, (size_t)needed);
        free(tmp);
    }

    Value result = make_string_value(out ? out : "", out_len);
    free(out);
    return result;
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

    Function *format_fn = xmalloc(sizeof(Function));
    format_fn->is_native = true;
    format_fn->native = native_string_format;
    table_set(string, make_string_value("format", 6), make_function(format_fn));

    table_freeze(string);
    runtime_ctx.string = string;
    return string;
}
