#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "runtime_sys.h"

static Value native_sys_cwd(int argc, Value *argv, EvalResult *err) {
    (void)argv;
    if (argc != 0) {
        runtime_set_error(err, "sys.cwd expects ()");
        return make_null();
    }
    char buf[1024];
    if (!getcwd(buf, sizeof(buf))) {
        runtime_set_error(err, "sys.cwd failed");
        return make_null();
    }
    return make_string_value(buf, strlen(buf));
}

static Value native_sys_platform(int argc, Value *argv, EvalResult *err) {
    (void)argv;
    if (argc != 0) {
        runtime_set_error(err, "sys.platform expects ()");
        return make_null();
    }
#if defined(__APPLE__)
    return make_string_value("darwin", 6);
#elif defined(_WIN32)
    return make_string_value("windows", 7);
#elif defined(__linux__)
    return make_string_value("linux", 5);
#else
    return make_string_value("unknown", 7);
#endif
}

static Value native_sys_exit(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_INT) {
        runtime_set_error(err, "sys.exit expects (int)");
        return make_null();
    }
    exit((int)argv[0].as.i);
}

static Table *build_sys_args(void) {
    if (runtime_ctx.sys_args) {
        return runtime_ctx.sys_args;
    }
    Table *nil = table_new();
    table_set(nil, make_string_value("isNil", 5), make_bool(true));
    table_freeze(nil);

    Value list = make_table(nil);
    for (int i = runtime_ctx.argc - 1; i >= 1; i--) {
        Table *node = table_new();
        table_set(node, make_string_value("isNil", 5), make_bool(false));
        table_set(node, make_string_value("head", 4),
                  make_string_value(runtime_ctx.argv[i], strlen(runtime_ctx.argv[i])));
        table_set(node, make_string_value("tail", 4), list);
        table_freeze(node);
        list = make_table(node);
    }
    runtime_ctx.sys_args = list.as.table;
    return runtime_ctx.sys_args;
}

static Table *build_sys_env(void) {
    if (runtime_ctx.sys_env) {
        return runtime_ctx.sys_env;
    }
    Table *env = table_new();
#if defined(__unix__) || defined(__APPLE__)
    extern char **environ;
    for (char **p = environ; p && *p; p++) {
        char *eq = strchr(*p, '=');
        if (!eq) {
            continue;
        }
        size_t klen = (size_t)(eq - *p);
        Value key = make_string_value(*p, klen);
        Value val = make_string_value(eq + 1, strlen(eq + 1));
        table_set(env, key, val);
    }
#endif
    table_freeze(env);
    runtime_ctx.sys_env = env;
    return env;
}

Table *runtime_sys_build(void) {
    if (runtime_ctx.sys) {
        return runtime_ctx.sys;
    }
    Table *sys = table_new();
    table_set(sys, make_string_value("args", 4), make_table(build_sys_args()));
    table_set(sys, make_string_value("env", 3), make_table(build_sys_env()));

    Function *cwd_fn = xmalloc(sizeof(Function));
    cwd_fn->is_native = true;
    cwd_fn->native = native_sys_cwd;
    table_set(sys, make_string_value("cwd", 3), make_function(cwd_fn));

    Function *platform_fn = xmalloc(sizeof(Function));
    platform_fn->is_native = true;
    platform_fn->native = native_sys_platform;
    table_set(sys, make_string_value("platform", 8), make_function(platform_fn));

    Function *exit_fn = xmalloc(sizeof(Function));
    exit_fn->is_native = true;
    exit_fn->native = native_sys_exit;
    table_set(sys, make_string_value("exit", 4), make_function(exit_fn));

    table_freeze(sys);
    runtime_ctx.sys = sys;
    return sys;
}
