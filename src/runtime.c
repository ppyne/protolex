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
    return false;
}
