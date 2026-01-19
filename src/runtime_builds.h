#ifndef PROTOLEX_RUNTIME_BUILDS_H
#define PROTOLEX_RUNTIME_BUILDS_H

#include "runtime_internal.h"

Table *runtime_io_build(void);
Table *runtime_time_build(void);
Table *runtime_sys_build(void);
Table *runtime_log_build(void);

#endif
