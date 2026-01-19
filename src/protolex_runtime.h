#ifndef PROTOLEX_RUNTIME_H
#define PROTOLEX_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct String {
    char *data;
    size_t len;
    uint32_t hash;
} String;

typedef enum {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_NULL,
    VAL_STRING,
    VAL_FUNCTION,
    VAL_TABLE,
    VAL_UNDEFINED
} ValueType;

struct Function;
struct Table;
struct Node;
struct Env;

typedef struct Value {
    ValueType type;
    union {
        int64_t i;
        double f;
        bool b;
        String *str;
        struct Function *fn;
        struct Table *table;
    } as;
} Value;

typedef struct {
    Value key;
    Value value;
    bool used;
    bool tombstone;
} Entry;

typedef struct {
    Entry *entries;
    size_t capacity;
    size_t count;
} Map;

typedef struct Table {
    Map map;
    struct Table *proto;
    bool frozen;
    int thaw_count;
} Table;

typedef struct Env {
    Map map;
    struct Env *parent;
} Env;

typedef struct EvalResult {
    bool is_exception;
    Value value;
} EvalResult;

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} StrList;

typedef Value (*NativeFn)(int argc, Value *argv, EvalResult *err);

typedef struct Function {
    bool is_native;
    int arity;
    StrList params;
    struct Node *body;
    Env *env;
    NativeFn native;
} Function;

void runtime_fatal(const char *msg);
void *xmalloc(size_t size);

Value make_int(int64_t v);
Value make_float(double v);
Value make_bool(bool v);
Value make_null(void);
Value make_undefined(void);
Value make_string_value(const char *s, size_t len);
Value make_table(Table *t);
Value make_function(Function *fn);

Table *table_new(void);
bool table_set(Table *t, Value key, Value value);
void table_freeze(Table *t);

void print_value(Value v);
void print_value_to(FILE *out, Value v);

EvalResult call_function(Value callee, int argc, Value *argv, const char *module_dir);

#endif
