#ifndef PROTOLEX_RUNTIME_LIB_H
#define PROTOLEX_RUNTIME_LIB_H

#include "protolex_runtime.h"

void runtime_init(int argc, char **argv, const char *module_dir);
bool runtime_import(const char *path, Env *env, Value *out);

#endif
