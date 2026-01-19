#include "runtime_log.h"

static Value native_log_info(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        runtime_set_error(err, "log.info expects (value)");
        return make_null();
    }
    print_value_to(stdout, argv[0]);
    fprintf(stdout, "\n");
    return make_null();
}

static Value native_log_warn(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        runtime_set_error(err, "log.warn expects (value)");
        return make_null();
    }
    print_value_to(stderr, argv[0]);
    fprintf(stderr, "\n");
    return make_null();
}

static Value native_log_error(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        runtime_set_error(err, "log.error expects (value)");
        return make_null();
    }
    print_value_to(stderr, argv[0]);
    fprintf(stderr, "\n");
    return make_null();
}

Table *runtime_log_build(void) {
    if (runtime_ctx.log) {
        return runtime_ctx.log;
    }
    Table *log = table_new();
    Function *info_fn = xmalloc(sizeof(Function));
    info_fn->is_native = true;
    info_fn->native = native_log_info;
    table_set(log, make_string_value("info", 4), make_function(info_fn));

    Function *warn_fn = xmalloc(sizeof(Function));
    warn_fn->is_native = true;
    warn_fn->native = native_log_warn;
    table_set(log, make_string_value("warn", 4), make_function(warn_fn));

    Function *error_fn = xmalloc(sizeof(Function));
    error_fn->is_native = true;
    error_fn->native = native_log_error;
    table_set(log, make_string_value("error", 5), make_function(error_fn));

    table_freeze(log);
    runtime_ctx.log = log;
    return log;
}
