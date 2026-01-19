#include <string.h>

#include "runtime_internal.h"
#include "runtime_builds.h"

RuntimeContext runtime_ctx = {0};

void runtime_set_error(EvalResult *err, const char *msg) {
    if (!err) {
        return;
    }
    err->is_exception = true;
    err->value = make_string_value(msg, strlen(msg));
}

void runtime_init(int argc, char **argv, const char *module_dir) {
    runtime_ctx.argc = argc;
    runtime_ctx.argv = argv;
    runtime_ctx.module_dir = module_dir;
}

bool runtime_import(const char *path, Env *env, Value *out) {
    (void)env;
    if (strcmp(path, "runtime/io") == 0 || strcmp(path, "runtime/io.plx") == 0) {
        *out = make_table(runtime_io_build());
        return true;
    }
    if (strcmp(path, "runtime/time") == 0 || strcmp(path, "runtime/time.plx") == 0) {
        *out = make_table(runtime_time_build());
        return true;
    }
    if (strcmp(path, "runtime/sys") == 0 || strcmp(path, "runtime/sys.plx") == 0) {
        *out = make_table(runtime_sys_build());
        return true;
    }
    if (strcmp(path, "runtime/log") == 0 || strcmp(path, "runtime/log.plx") == 0) {
        *out = make_table(runtime_log_build());
        return true;
    }
    if (strcmp(path, "runtime/string") == 0 || strcmp(path, "runtime/string.plx") == 0) {
        *out = make_table(runtime_string_build());
        return true;
    }
    if (strcmp(path, "runtime/int") == 0 || strcmp(path, "runtime/int.plx") == 0) {
        *out = make_table(runtime_int_build());
        return true;
    }
    if (strcmp(path, "runtime/float") == 0 || strcmp(path, "runtime/float.plx") == 0) {
        *out = make_table(runtime_float_build());
        return true;
    }
    if (strcmp(path, "runtime/math") == 0 || strcmp(path, "runtime/math.plx") == 0) {
        *out = make_table(runtime_math_build());
        return true;
    }
    return false;
}
