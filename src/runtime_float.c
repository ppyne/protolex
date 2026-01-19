#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "runtime_float.h"

static Value native_float_abs(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.abs expects (float)");
        return make_null();
    }
    return make_float(fabs(argv[0].as.f));
}

static Value native_float_min(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_FLOAT || argv[1].type != VAL_FLOAT) {
        runtime_set_error(err, "float.min expects (float, float)");
        return make_null();
    }
    return make_float(argv[0].as.f < argv[1].as.f ? argv[0].as.f : argv[1].as.f);
}

static Value native_float_max(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_FLOAT || argv[1].type != VAL_FLOAT) {
        runtime_set_error(err, "float.max expects (float, float)");
        return make_null();
    }
    return make_float(argv[0].as.f > argv[1].as.f ? argv[0].as.f : argv[1].as.f);
}

static Value native_float_round(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.round expects (float)");
        return make_null();
    }
    return make_float(round(argv[0].as.f));
}

static Value native_float_floor(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.floor expects (float)");
        return make_null();
    }
    return make_float(floor(argv[0].as.f));
}

static Value native_float_ceil(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.ceil expects (float)");
        return make_null();
    }
    return make_float(ceil(argv[0].as.f));
}

static Value native_float_pow(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_FLOAT || argv[1].type != VAL_FLOAT) {
        runtime_set_error(err, "float.pow expects (float, float)");
        return make_null();
    }
    return make_float(pow(argv[0].as.f, argv[1].as.f));
}

static Value native_float_sqrt(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.sqrt expects (float)");
        return make_null();
    }
    return make_float(sqrt(argv[0].as.f));
}

static Value native_float_parse(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_STRING) {
        runtime_set_error(err, "float.parse expects (string)");
        return make_null();
    }
    char *end = NULL;
    double val = strtod(argv[0].as.str->data, &end);
    if (!end || (size_t)(end - argv[0].as.str->data) != argv[0].as.str->len) {
        runtime_set_error(err, "float.parse invalid");
        return make_null();
    }
    return make_float(val);
}

static Value native_float_to_string(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "float.toString expects (float)");
        return make_null();
    }
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%.17g", argv[0].as.f);
    if (len < 0) {
        runtime_set_error(err, "float.toString failed");
        return make_null();
    }
    return make_string_value(buf, (size_t)len);
}

Table *runtime_float_build(void) {
    if (runtime_ctx.floatlib) {
        return runtime_ctx.floatlib;
    }
    Table *tbl = table_new();

    Function *abs_fn = xmalloc(sizeof(Function));
    abs_fn->is_native = true;
    abs_fn->native = native_float_abs;
    table_set(tbl, make_string_value("abs", 3), make_function(abs_fn));

    Function *min_fn = xmalloc(sizeof(Function));
    min_fn->is_native = true;
    min_fn->native = native_float_min;
    table_set(tbl, make_string_value("min", 3), make_function(min_fn));

    Function *max_fn = xmalloc(sizeof(Function));
    max_fn->is_native = true;
    max_fn->native = native_float_max;
    table_set(tbl, make_string_value("max", 3), make_function(max_fn));

    Function *round_fn = xmalloc(sizeof(Function));
    round_fn->is_native = true;
    round_fn->native = native_float_round;
    table_set(tbl, make_string_value("round", 5), make_function(round_fn));

    Function *floor_fn = xmalloc(sizeof(Function));
    floor_fn->is_native = true;
    floor_fn->native = native_float_floor;
    table_set(tbl, make_string_value("floor", 5), make_function(floor_fn));

    Function *ceil_fn = xmalloc(sizeof(Function));
    ceil_fn->is_native = true;
    ceil_fn->native = native_float_ceil;
    table_set(tbl, make_string_value("ceil", 4), make_function(ceil_fn));

    Function *pow_fn = xmalloc(sizeof(Function));
    pow_fn->is_native = true;
    pow_fn->native = native_float_pow;
    table_set(tbl, make_string_value("pow", 3), make_function(pow_fn));

    Function *sqrt_fn = xmalloc(sizeof(Function));
    sqrt_fn->is_native = true;
    sqrt_fn->native = native_float_sqrt;
    table_set(tbl, make_string_value("sqrt", 4), make_function(sqrt_fn));

    Function *parse_fn = xmalloc(sizeof(Function));
    parse_fn->is_native = true;
    parse_fn->native = native_float_parse;
    table_set(tbl, make_string_value("parse", 5), make_function(parse_fn));

    Function *to_str_fn = xmalloc(sizeof(Function));
    to_str_fn->is_native = true;
    to_str_fn->native = native_float_to_string;
    table_set(tbl, make_string_value("toString", 8), make_function(to_str_fn));

    table_freeze(tbl);
    runtime_ctx.floatlib = tbl;
    return tbl;
}
