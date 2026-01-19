#ifndef PROTOLEX_RUNTIME_INTERNAL_H
#define PROTOLEX_RUNTIME_INTERNAL_H

#include "runtime.h"

typedef struct {
    int argc;
    char **argv;
    const char *module_dir;
    Table *io;
    Table *time;
    Table *sys;
    Table *log;
    Table *sys_args;
    Table *sys_env;
} RuntimeContext;

extern RuntimeContext runtime_ctx;

void runtime_set_error(EvalResult *err, const char *msg);


#endif
