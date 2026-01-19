#include <math.h>
#include <stdlib.h>

#include "runtime_math.h"

static Value native_math_sin(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "math.sin expects (float)");
        return make_null();
    }
    return make_float(sin(argv[0].as.f));
}

static Value native_math_cos(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "math.cos expects (float)");
        return make_null();
    }
    return make_float(cos(argv[0].as.f));
}

static Value native_math_tan(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "math.tan expects (float)");
        return make_null();
    }
    return make_float(tan(argv[0].as.f));
}

static Value native_math_exp(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "math.exp expects (float)");
        return make_null();
    }
    return make_float(exp(argv[0].as.f));
}

static Value native_math_log(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_FLOAT) {
        runtime_set_error(err, "math.log expects (float)");
        return make_null();
    }
    return make_float(log(argv[0].as.f));
}

static Value native_math_random(int argc, Value *argv, EvalResult *err) {
    (void)argv;
    if (argc != 0) {
        runtime_set_error(err, "math.random expects ()");
        return make_null();
    }
    double r = (double)rand() / (double)RAND_MAX;
    return make_float(r);
}

Table *runtime_math_build(void) {
    if (runtime_ctx.mathlib) {
        return runtime_ctx.mathlib;
    }
    Table *tbl = table_new();

    Function *sin_fn = xmalloc(sizeof(Function));
    sin_fn->is_native = true;
    sin_fn->native = native_math_sin;
    table_set(tbl, make_string_value("sin", 3), make_function(sin_fn));

    Function *cos_fn = xmalloc(sizeof(Function));
    cos_fn->is_native = true;
    cos_fn->native = native_math_cos;
    table_set(tbl, make_string_value("cos", 3), make_function(cos_fn));

    Function *tan_fn = xmalloc(sizeof(Function));
    tan_fn->is_native = true;
    tan_fn->native = native_math_tan;
    table_set(tbl, make_string_value("tan", 3), make_function(tan_fn));

    Function *exp_fn = xmalloc(sizeof(Function));
    exp_fn->is_native = true;
    exp_fn->native = native_math_exp;
    table_set(tbl, make_string_value("exp", 3), make_function(exp_fn));

    Function *log_fn = xmalloc(sizeof(Function));
    log_fn->is_native = true;
    log_fn->native = native_math_log;
    table_set(tbl, make_string_value("log", 3), make_function(log_fn));

    Function *rand_fn = xmalloc(sizeof(Function));
    rand_fn->is_native = true;
    rand_fn->native = native_math_random;
    table_set(tbl, make_string_value("random", 6), make_function(rand_fn));

    table_set(tbl, make_string_value("pi", 2), make_float(3.141592653589793));
    table_set(tbl, make_string_value("e", 1), make_float(2.718281828459045));

    table_freeze(tbl);
    runtime_ctx.mathlib = tbl;
    return tbl;
}
