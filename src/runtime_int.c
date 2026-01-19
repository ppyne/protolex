#include <limits.h>
#include <stdlib.h>

#include "runtime_int.h"

static Value native_int_abs(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_INT) {
        runtime_set_error(err, "int.abs expects (int)");
        return make_null();
    }
    int64_t v = argv[0].as.i;
    if (v == INT64_MIN) {
        runtime_set_error(err, "int.abs overflow");
        return make_null();
    }
    return make_int(v < 0 ? -v : v);
}

static Value native_int_min(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_INT || argv[1].type != VAL_INT) {
        runtime_set_error(err, "int.min expects (int, int)");
        return make_null();
    }
    return make_int(argv[0].as.i < argv[1].as.i ? argv[0].as.i : argv[1].as.i);
}

static Value native_int_max(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_INT || argv[1].type != VAL_INT) {
        runtime_set_error(err, "int.max expects (int, int)");
        return make_null();
    }
    return make_int(argv[0].as.i > argv[1].as.i ? argv[0].as.i : argv[1].as.i);
}

static Value native_int_clamp(int argc, Value *argv, EvalResult *err) {
    if (argc != 3 || argv[0].type != VAL_INT ||
        argv[1].type != VAL_INT || argv[2].type != VAL_INT) {
        runtime_set_error(err, "int.clamp expects (int, int, int)");
        return make_null();
    }
    int64_t x = argv[0].as.i;
    int64_t min = argv[1].as.i;
    int64_t max = argv[2].as.i;
    if (min > max) {
        runtime_set_error(err, "int.clamp invalid range");
        return make_null();
    }
    if (x < min) x = min;
    if (x > max) x = max;
    return make_int(x);
}

static Value native_int_pow(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_INT || argv[1].type != VAL_INT) {
        runtime_set_error(err, "int.pow expects (int, int)");
        return make_null();
    }
    int64_t base = argv[0].as.i;
    int64_t exp = argv[1].as.i;
    if (exp < 0) {
        runtime_set_error(err, "int.pow expects non-negative exponent");
        return make_null();
    }
    int64_t result = 1;
    for (int64_t i = 0; i < exp; i++) {
        result *= base;
    }
    return make_int(result);
}

static Value native_int_parse(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "int.parse expects (string)");
        return make_null();
    }
    char *end = NULL;
    long long val = strtoll(argv[0].as.str->data, &end, 10);
    if (!end || (size_t)(end - argv[0].as.str->data) != argv[0].as.str->len) {
        runtime_set_error(err, "int.parse invalid");
        return make_null();
    }
    return make_int((int64_t)val);
}

static Value native_int_to_string(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_INT) {
        runtime_set_error(err, "int.toString expects (int)");
        return make_null();
    }
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%lld", (long long)argv[0].as.i);
    if (len < 0) {
        runtime_set_error(err, "int.toString failed");
        return make_null();
    }
    return make_string_value(buf, (size_t)len);
}

Table *runtime_int_build(void) {
    if (runtime_ctx.intlib) {
        return runtime_ctx.intlib;
    }
    Table *tbl = table_new();

    Function *abs_fn = xmalloc(sizeof(Function));
    abs_fn->is_native = true;
    abs_fn->native = native_int_abs;
    table_set(tbl, make_string_value("abs", 3), make_function(abs_fn));

    Function *min_fn = xmalloc(sizeof(Function));
    min_fn->is_native = true;
    min_fn->native = native_int_min;
    table_set(tbl, make_string_value("min", 3), make_function(min_fn));

    Function *max_fn = xmalloc(sizeof(Function));
    max_fn->is_native = true;
    max_fn->native = native_int_max;
    table_set(tbl, make_string_value("max", 3), make_function(max_fn));

    Function *clamp_fn = xmalloc(sizeof(Function));
    clamp_fn->is_native = true;
    clamp_fn->native = native_int_clamp;
    table_set(tbl, make_string_value("clamp", 5), make_function(clamp_fn));

    Function *pow_fn = xmalloc(sizeof(Function));
    pow_fn->is_native = true;
    pow_fn->native = native_int_pow;
    table_set(tbl, make_string_value("pow", 3), make_function(pow_fn));

    Function *parse_fn = xmalloc(sizeof(Function));
    parse_fn->is_native = true;
    parse_fn->native = native_int_parse;
    table_set(tbl, make_string_value("parse", 5), make_function(parse_fn));

    Function *to_str_fn = xmalloc(sizeof(Function));
    to_str_fn->is_native = true;
    to_str_fn->native = native_int_to_string;
    table_set(tbl, make_string_value("toString", 8), make_function(to_str_fn));

    table_freeze(tbl);
    runtime_ctx.intlib = tbl;
    return tbl;
}
