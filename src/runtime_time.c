#include <time.h>

#include "runtime_time.h"

static Value native_time_now(int argc, Value *argv, EvalResult *err) {
    (void)argv;
    if (argc != 0) {
        runtime_set_error(err, "time.now expects ()");
        return make_null();
    }
    struct timespec ts;
#if defined(CLOCK_REALTIME)
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        int64_t ms = (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
        return make_int(ms);
    }
#endif
    runtime_set_error(err, "time.now failed");
    return make_null();
}

static Value native_time_monotonic(int argc, Value *argv, EvalResult *err) {
    (void)argv;
    if (argc != 0) {
        runtime_set_error(err, "time.monotonic expects ()");
        return make_null();
    }
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        int64_t ms = (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
        return make_int(ms);
    }
#endif
    runtime_set_error(err, "time.monotonic failed");
    return make_null();
}

static Value native_time_sleep(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_INT) {
        runtime_set_error(err, "time.sleep expects (int)");
        return make_null();
    }
    int64_t ms = argv[0].as.i;
    if (ms < 0) {
        runtime_set_error(err, "time.sleep expects non-negative");
        return make_null();
    }
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
    return make_null();
}

Table *runtime_time_build(void) {
    if (runtime_ctx.time) {
        return runtime_ctx.time;
    }
    Table *time = table_new();
    Function *now_fn = xmalloc(sizeof(Function));
    now_fn->is_native = true;
    now_fn->native = native_time_now;
    table_set(time, make_string_value("now", 3), make_function(now_fn));

    Function *mono_fn = xmalloc(sizeof(Function));
    mono_fn->is_native = true;
    mono_fn->native = native_time_monotonic;
    table_set(time, make_string_value("monotonic", 9), make_function(mono_fn));

    Function *sleep_fn = xmalloc(sizeof(Function));
    sleep_fn->is_native = true;
    sleep_fn->native = native_time_sleep;
    table_set(time, make_string_value("sleep", 5), make_function(sleep_fn));

    table_freeze(time);
    runtime_ctx.time = time;
    return time;
}
