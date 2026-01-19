#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "runtime_io.h"

typedef struct {
    Table *object;
    FILE *file;
    bool owned;
} FileEntry;

static FileEntry *files = NULL;
static size_t file_count = 0;
static size_t file_capacity = 0;

static void file_registry_add(Table *obj, FILE *file, bool owned) {
    if (file_count == file_capacity) {
        size_t cap = file_capacity ? file_capacity * 2 : 16;
        files = realloc(files, cap * sizeof(FileEntry));
        if (!files) {
            runtime_fatal("out of memory");
        }
        file_capacity = cap;
    }
    files[file_count++] = (FileEntry){obj, file, owned};
}

static FileEntry *file_registry_find(Table *obj) {
    for (size_t i = 0; i < file_count; i++) {
        if (files[i].object == obj) {
            return &files[i];
        }
    }
    return NULL;
}

static Value native_io_open(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_STRING || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "io.open expects (string, string)");
        return make_null();
    }
    FILE *f = fopen(argv[0].as.str->data, argv[1].as.str->data);
    if (!f) {
        runtime_set_error(err, strerror(errno));
        return make_null();
    }
    Table *file_obj = table_new();
    table_freeze(file_obj);
    file_registry_add(file_obj, f, true);
    return make_table(file_obj);
}

static Value native_io_read(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_TABLE) {
        runtime_set_error(err, "io.read expects (file)");
        return make_null();
    }
    FileEntry *entry = file_registry_find(argv[0].as.table);
    if (!entry || !entry->file) {
        runtime_set_error(err, "invalid file");
        return make_null();
    }
    size_t cap = 4096;
    size_t len = 0;
    char *buf = xmalloc(cap);
    while (true) {
        size_t n = fread(buf + len, 1, cap - len, entry->file);
        len += n;
        if (n == 0) {
            if (ferror(entry->file)) {
                free(buf);
                runtime_set_error(err, "io.read failed");
                return make_null();
            }
            break;
        }
        if (len == cap) {
            cap *= 2;
            buf = realloc(buf, cap);
            if (!buf) {
                runtime_fatal("out of memory");
            }
        }
    }
    Value out = make_string_value(buf, len);
    free(buf);
    return out;
}

static Value native_io_write(int argc, Value *argv, EvalResult *err) {
    if (argc != 2 || argv[0].type != VAL_TABLE || argv[1].type != VAL_STRING) {
        runtime_set_error(err, "io.write expects (file, string)");
        return make_null();
    }
    FileEntry *entry = file_registry_find(argv[0].as.table);
    if (!entry || !entry->file) {
        runtime_set_error(err, "invalid file");
        return make_null();
    }
    size_t written = fwrite(argv[1].as.str->data, 1, argv[1].as.str->len, entry->file);
    if (written != argv[1].as.str->len) {
        runtime_set_error(err, "io.write failed");
        return make_null();
    }
    return make_null();
}

static Value native_io_close(int argc, Value *argv, EvalResult *err) {
    if (argc != 1 || argv[0].type != VAL_TABLE) {
        runtime_set_error(err, "io.close expects (file)");
        return make_null();
    }
    FileEntry *entry = file_registry_find(argv[0].as.table);
    if (!entry || !entry->file) {
        runtime_set_error(err, "invalid file");
        return make_null();
    }
    if (entry->owned) {
        fclose(entry->file);
    }
    entry->file = NULL;
    return make_null();
}

Table *runtime_io_build(void) {
    if (runtime_ctx.io) {
        return runtime_ctx.io;
    }
    Table *io = table_new();
    Function *open_fn = xmalloc(sizeof(Function));
    open_fn->is_native = true;
    open_fn->native = native_io_open;
    table_set(io, make_string_value("open", 4), make_function(open_fn));

    Function *read_fn = xmalloc(sizeof(Function));
    read_fn->is_native = true;
    read_fn->native = native_io_read;
    table_set(io, make_string_value("read", 4), make_function(read_fn));

    Function *write_fn = xmalloc(sizeof(Function));
    write_fn->is_native = true;
    write_fn->native = native_io_write;
    table_set(io, make_string_value("write", 5), make_function(write_fn));

    Function *close_fn = xmalloc(sizeof(Function));
    close_fn->is_native = true;
    close_fn->native = native_io_close;
    table_set(io, make_string_value("close", 5), make_function(close_fn));

    Table *stdin_obj = table_new();
    table_freeze(stdin_obj);
    file_registry_add(stdin_obj, stdin, false);
    table_set(io, make_string_value("stdin", 5), make_table(stdin_obj));

    Table *stdout_obj = table_new();
    table_freeze(stdout_obj);
    file_registry_add(stdout_obj, stdout, false);
    table_set(io, make_string_value("stdout", 6), make_table(stdout_obj));

    Table *stderr_obj = table_new();
    table_freeze(stderr_obj);
    file_registry_add(stderr_obj, stderr, false);
    table_set(io, make_string_value("stderr", 6), make_table(stderr_obj));

    table_freeze(io);
    runtime_ctx.io = io;
    return io;
}
