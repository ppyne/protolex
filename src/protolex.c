#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "protolex_runtime.h"
#include "runtime.h"

typedef struct Node Node;


typedef enum {
    NODE_BLOCK,
    NODE_LITERAL,
    NODE_VAR,
    NODE_ASSIGN,
    NODE_BINARY,
    NODE_UNARY,
    NODE_CALL,
    NODE_DOT,
    NODE_INDEX,
    NODE_IF,
    NODE_FN,
    NODE_IMPORT,
    NODE_MUTATE,
    NODE_UNDEFINE,
    NODE_TRY,
    NODE_THROW,
    NODE_TABLE
} NodeType;

typedef struct {
    Node **items;
    size_t count;
    size_t capacity;
} NodeList;

typedef struct {
    char **keys;
    Node **values;
    size_t count;
    size_t capacity;
} TableLiteral;

struct Node {
    NodeType type;
    int line;
    int col;
    union {
        Value literal;
        struct {
            char *name;
        } var;
        struct {
            Node *target;
            Node *value;
        } assign;
        struct {
            char *op;
            Node *left;
            Node *right;
        } binary;
        struct {
            char *op;
            Node *expr;
        } unary;
        struct {
            Node *callee;
            NodeList args;
        } call;
        struct {
            Node *object;
            char *name;
        } dot;
        struct {
            Node *object;
            Node *index;
        } index;
        struct {
            Node *cond;
            Node *then_branch;
            Node *else_branch;
        } if_expr;
        struct {
            StrList params;
            Node *body;
        } fn;
        struct {
            char *name;
            char *path;
        } import_stmt;
        struct {
            Node *target;
            Node *body;
        } mutate;
        struct {
            Node *target;
        } undefine;
        struct {
            Node *try_block;
            char *catch_name;
            bool catch_any;
            Node *catch_block;
            Node *finally_block;
        } try_expr;
        struct {
            Node *expr;
        } throw_expr;
        struct {
            TableLiteral items;
        } table;
        struct {
            NodeList statements;
        } block;
    } as;
};

typedef enum {
    TOK_EOF,
    TOK_IDENT,
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,
    TOK_NEWLINE,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_COMMA,
    TOK_DOT,
    TOK_ASSIGN,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_FOR,
    TOK_IN,
    TOK_FN,
    TOK_IMPORT,
    TOK_FROM,
    TOK_MUTATE,
    TOK_UNDEFINE,
    TOK_TRY,
    TOK_CATCH,
    TOK_FINALLY,
    TOK_THROW,
    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL
} TokenType;

typedef struct {
    TokenType type;
    int line;
    int col;
    char *lexeme;
    double number;
} Token;

typedef struct {
    Token *items;
    size_t count;
    size_t capacity;
} TokenList;

typedef struct {
    const char *src;
    size_t len;
    size_t pos;
    int line;
    int col;
    int paren_depth;
    int bracket_depth;
} Lexer;

typedef struct {
    TokenList tokens;
    size_t current;
} Parser;

static Parser *g_parse_parser = NULL;
static const char *g_parse_file = NULL;

static Token *peek(Parser *p);

static void set_parse_context(Parser *p, const char *file) {
    g_parse_parser = p;
    g_parse_file = file;
}

static void clear_parse_context(void) {
    g_parse_parser = NULL;
    g_parse_file = NULL;
}

void runtime_fatal(const char *msg) {
    if (g_parse_parser) {
        Token *tok = peek(g_parse_parser);
        if (g_parse_file) {
            fprintf(stderr, "fatal: %s at %s:%d:%d\n", msg, g_parse_file, tok->line, tok->col);
        } else {
            fprintf(stderr, "fatal: %s at %d:%d\n", msg, tok->line, tok->col);
        }
    } else {
        fprintf(stderr, "fatal: %s\n", msg);
    }
    exit(1);
}

void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        runtime_fatal("out of memory");
    }
    return p;
}

static char *xstrndup(const char *s, size_t n) {
    char *out = xmalloc(n + 1);
    memcpy(out, s, n);
    out[n] = '\0';
    return out;
}

static uint32_t hash_bytes(const uint8_t *data, size_t len) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

static String *make_string(const char *s, size_t len) {
    String *str = xmalloc(sizeof(String));
    str->data = xstrndup(s, len);
    str->len = len;
    str->hash = hash_bytes((const uint8_t *)str->data, len);
    return str;
}

Value make_int(int64_t v) {
    Value val;
    val.type = VAL_INT;
    val.as.i = v;
    return val;
}

Value make_float(double v) {
    Value val;
    val.type = VAL_FLOAT;
    val.as.f = v;
    return val;
}

Value make_bool(bool v) {
    Value val;
    val.type = VAL_BOOL;
    val.as.b = v;
    return val;
}

Value make_null(void) {
    Value val;
    val.type = VAL_NULL;
    return val;
}

Value make_undefined(void) {
    Value val;
    val.type = VAL_UNDEFINED;
    return val;
}

Value make_string_value(const char *s, size_t len) {
    Value val;
    val.type = VAL_STRING;
    val.as.str = make_string(s, len);
    return val;
}

Value make_table(Table *t) {
    Value val;
    val.type = VAL_TABLE;
    val.as.table = t;
    return val;
}

Value make_function(Function *fn) {
    Value val;
    val.type = VAL_FUNCTION;
    val.as.fn = fn;
    return val;
}

static bool value_is_truthy(Value v) {
    if (v.type == VAL_BOOL) {
        return v.as.b;
    }
    if (v.type == VAL_NULL || v.type == VAL_UNDEFINED) {
        return false;
    }
    return true;
}

static bool value_equal(Value a, Value b) {
    if (a.type != b.type) {
        if ((a.type == VAL_INT && b.type == VAL_FLOAT) ||
            (a.type == VAL_FLOAT && b.type == VAL_INT)) {
            double av = (a.type == VAL_FLOAT) ? a.as.f : (double)a.as.i;
            double bv = (b.type == VAL_FLOAT) ? b.as.f : (double)b.as.i;
            return av == bv;
        }
        return false;
    }
    switch (a.type) {
    case VAL_INT:
        return a.as.i == b.as.i;
    case VAL_FLOAT:
        return a.as.f == b.as.f;
    case VAL_BOOL:
        return a.as.b == b.as.b;
    case VAL_NULL:
    case VAL_UNDEFINED:
        return true;
    case VAL_STRING:
        if (a.as.str->len != b.as.str->len) {
            return false;
        }
        return memcmp(a.as.str->data, b.as.str->data, a.as.str->len) == 0;
    case VAL_TABLE:
        return a.as.table == b.as.table;
    case VAL_FUNCTION:
        return a.as.fn == b.as.fn;
    }
    return false;
}

static uint32_t value_hash(Value v) {
    switch (v.type) {
    case VAL_INT:
        return (uint32_t)(v.as.i ^ (v.as.i >> 32));
    case VAL_FLOAT: {
        uint64_t bits;
        memcpy(&bits, &v.as.f, sizeof(bits));
        return (uint32_t)(bits ^ (bits >> 32));
    }
    case VAL_BOOL:
        return v.as.b ? 0x9e3779b1u : 0x85ebca6bu;
    case VAL_NULL:
        return 0x27d4eb2du;
    case VAL_UNDEFINED:
        return 0x165667b1u;
    case VAL_STRING:
        return v.as.str->hash;
    case VAL_TABLE:
        return (uint32_t)(uintptr_t)v.as.table;
    case VAL_FUNCTION:
        return (uint32_t)(uintptr_t)v.as.fn;
    }
    return 0;
}

static bool value_is_number(Value v) {
    return v.type == VAL_INT || v.type == VAL_FLOAT;
}

static double value_as_double(Value v) {
    return v.type == VAL_FLOAT ? v.as.f : (double)v.as.i;
}

static void map_init(Map *map) {
    map->capacity = 16;
    map->count = 0;
    map->entries = calloc(map->capacity, sizeof(Entry));
    if (!map->entries) {
        runtime_fatal("out of memory");
    }
}

static void map_free(Map *map) {
    free(map->entries);
    map->entries = NULL;
    map->capacity = 0;
    map->count = 0;
}

static void map_resize(Map *map, size_t new_capacity) {
    Entry *old = map->entries;
    size_t old_cap = map->capacity;

    map->entries = calloc(new_capacity, sizeof(Entry));
    if (!map->entries) {
        runtime_fatal("out of memory");
    }
    map->capacity = new_capacity;
    map->count = 0;

    for (size_t i = 0; i < old_cap; i++) {
        if (old[i].used && !old[i].tombstone) {
            size_t idx = value_hash(old[i].key) % map->capacity;
            while (map->entries[idx].used) {
                idx = (idx + 1) % map->capacity;
            }
            map->entries[idx] = old[i];
            map->count++;
        }
    }
    free(old);
}

static bool map_set(Map *map, Value key, Value value) {
    if ((map->count + 1) * 100 / map->capacity > 70) {
        map_resize(map, map->capacity * 2);
    }
    size_t idx = value_hash(key) % map->capacity;
    size_t first_tombstone = (size_t)-1;

    while (map->entries[idx].used) {
        if (!map->entries[idx].tombstone && value_equal(map->entries[idx].key, key)) {
            map->entries[idx].value = value;
            return false;
        }
        if (map->entries[idx].tombstone && first_tombstone == (size_t)-1) {
            first_tombstone = idx;
        }
        idx = (idx + 1) % map->capacity;
    }
    size_t target = (first_tombstone != (size_t)-1) ? first_tombstone : idx;
    map->entries[target].used = true;
    map->entries[target].tombstone = false;
    map->entries[target].key = key;
    map->entries[target].value = value;
    map->count++;
    return true;
}

static bool map_get(Map *map, Value key, Value *out) {
    size_t idx = value_hash(key) % map->capacity;
    while (map->entries[idx].used) {
        if (!map->entries[idx].tombstone && value_equal(map->entries[idx].key, key)) {
            *out = map->entries[idx].value;
            return true;
        }
        idx = (idx + 1) % map->capacity;
    }
    return false;
}

static bool map_has(Map *map, Value key) {
    Value out;
    return map_get(map, key, &out);
}

static bool map_delete(Map *map, Value key) {
    size_t idx = value_hash(key) % map->capacity;
    while (map->entries[idx].used) {
        if (!map->entries[idx].tombstone && value_equal(map->entries[idx].key, key)) {
            map->entries[idx].tombstone = true;
            map->count--;
            return true;
        }
        idx = (idx + 1) % map->capacity;
    }
    return false;
}

Table *table_new(void) {
    Table *t = xmalloc(sizeof(Table));
    map_init(&t->map);
    t->proto = NULL;
    t->frozen = false;
    t->thaw_count = 0;
    return t;
}

static bool table_has_proto_cycle(Table *self, Table *candidate) {
    for (Table *p = candidate; p != NULL; p = p->proto) {
        if (p == self) {
            return true;
        }
    }
    return false;
}

static bool table_set_proto(Table *self, Value v) {
    if (v.type == VAL_NULL) {
        self->proto = NULL;
        return true;
    }
    if (v.type != VAL_TABLE) {
        return false;
    }
    if (table_has_proto_cycle(self, v.as.table)) {
        runtime_fatal("prototype cycle");
    }
    self->proto = v.as.table;
    return true;
}

static bool table_can_mutate(Table *t) {
    return !t->frozen || t->thaw_count > 0;
}

void table_freeze(Table *t) {
    t->frozen = true;
}

typedef struct {
    Table **items;
    size_t count;
    size_t capacity;
} TableStack;

static void stack_push(TableStack *stack, Table *t) {
    if (stack->count == stack->capacity) {
        size_t cap = stack->capacity ? stack->capacity * 2 : 16;
        stack->items = realloc(stack->items, cap * sizeof(Table *));
        if (!stack->items) {
            runtime_fatal("out of memory");
        }
        stack->capacity = cap;
    }
    stack->items[stack->count++] = t;
}

static Table *stack_pop(TableStack *stack) {
    return stack->items[--stack->count];
}

static void table_adjust_thaw(Table *root, int delta) {
    TableStack stack = {0};
    Map visited;
    map_init(&visited);

    stack_push(&stack, root);
    while (stack.count > 0) {
        Table *t = stack_pop(&stack);
        Value key = make_table(t);
        if (map_has(&visited, key)) {
            continue;
        }
        map_set(&visited, key, make_bool(true));
        t->thaw_count += delta;

        for (size_t i = 0; i < t->map.capacity; i++) {
            Entry *e = &t->map.entries[i];
            if (e->used && !e->tombstone && e->value.type == VAL_TABLE) {
                stack_push(&stack, e->value.as.table);
            }
        }
    }

    map_free(&visited);
    free(stack.items);
}

bool table_set(Table *t, Value key, Value value) {
    if (!table_can_mutate(t)) {
        return false;
    }
    map_set(&t->map, key, value);
    return true;
}

static bool table_delete(Table *t, Value key) {
    if (!table_can_mutate(t)) {
        return false;
    }
    map_delete(&t->map, key);
    return true;
}

static Value table_get(Table *t, Value key) {
    Value out;
    for (Table *cur = t; cur != NULL; cur = cur->proto) {
        if (map_get(&cur->map, key, &out)) {
            return out;
        }
    }
    return make_undefined();
}

static bool table_has_local(Table *t, Value key) {
    return map_has(&t->map, key);
}

static Env *env_new(Env *parent) {
    Env *env = xmalloc(sizeof(Env));
    map_init(&env->map);
    env->parent = parent;
    return env;
}

static bool env_get(Env *env, const char *name, Value *out) {
    Value key = make_string_value(name, strlen(name));
    for (Env *cur = env; cur != NULL; cur = cur->parent) {
        if (map_get(&cur->map, key, out)) {
            return true;
        }
    }
    return false;
}

static void env_define(Env *env, const char *name, Value value) {
    Value key = make_string_value(name, strlen(name));
    map_set(&env->map, key, value);
}

static bool env_assign(Env *env, const char *name, Value value) {
    Value key = make_string_value(name, strlen(name));
    for (Env *cur = env; cur != NULL; cur = cur->parent) {
        if (map_has(&cur->map, key)) {
            map_set(&cur->map, key, value);
            return true;
        }
    }
    map_set(&env->map, key, value);
    return true;
}

static EvalResult ok(Value v) {
    EvalResult r;
    r.is_exception = false;
    r.value = v;
    return r;
}

static EvalResult exception(Value v) {
    EvalResult r;
    r.is_exception = true;
    r.value = v;
    return r;
}

static EvalResult error_msg(const char *msg) {
    return exception(make_string_value(msg, strlen(msg)));
}

void print_value_to(FILE *out, Value v) {
    switch (v.type) {
    case VAL_INT:
        fprintf(out, "%lld", (long long)v.as.i);
        break;
    case VAL_FLOAT:
        fprintf(out, "%f", v.as.f);
        break;
    case VAL_BOOL:
        fprintf(out, v.as.b ? "true" : "false");
        break;
    case VAL_NULL:
        fprintf(out, "null");
        break;
    case VAL_UNDEFINED:
        fprintf(out, "undefined");
        break;
    case VAL_STRING:
        fprintf(out, "%s", v.as.str->data);
        break;
    case VAL_TABLE:
        fprintf(out, "<table>");
        break;
    case VAL_FUNCTION:
        fprintf(out, "<function>");
        break;
    }
}

void print_value(Value v) {
    print_value_to(stderr, v);
}

static void node_list_init(NodeList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void node_list_push(NodeList *list, Node *node) {
    if (list->count == list->capacity) {
        size_t cap = list->capacity ? list->capacity * 2 : 8;
        list->items = realloc(list->items, cap * sizeof(Node *));
        if (!list->items) {
            runtime_fatal("out of memory");
        }
        list->capacity = cap;
    }
    list->items[list->count++] = node;
}

static void str_list_init(StrList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void str_list_push(StrList *list, char *s) {
    if (list->count == list->capacity) {
        size_t cap = list->capacity ? list->capacity * 2 : 8;
        list->items = realloc(list->items, cap * sizeof(char *));
        if (!list->items) {
            runtime_fatal("out of memory");
        }
        list->capacity = cap;
    }
    list->items[list->count++] = s;
}

static void table_literal_init(TableLiteral *t) {
    t->keys = NULL;
    t->values = NULL;
    t->count = 0;
    t->capacity = 0;
}

static void table_literal_push(TableLiteral *t, char *key, Node *value) {
    if (t->count == t->capacity) {
        size_t cap = t->capacity ? t->capacity * 2 : 8;
        t->keys = realloc(t->keys, cap * sizeof(char *));
        t->values = realloc(t->values, cap * sizeof(Node *));
        if (!t->keys || !t->values) {
            runtime_fatal("out of memory");
        }
        t->capacity = cap;
    }
    t->keys[t->count] = key;
    t->values[t->count] = value;
    t->count++;
}

static Node *node_new(NodeType type, int line, int col) {
    Node *node = xmalloc(sizeof(Node));
    node->type = type;
    node->line = line;
    node->col = col;
    return node;
}

static bool is_ident_start(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

static bool is_ident_part(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static int digit_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

static void token_list_init(TokenList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void token_list_push(TokenList *list, Token tok) {
    if (list->count == list->capacity) {
        size_t cap = list->capacity ? list->capacity * 2 : 64;
        list->items = realloc(list->items, cap * sizeof(Token));
        if (!list->items) {
            runtime_fatal("out of memory");
        }
        list->capacity = cap;
    }
    list->items[list->count++] = tok;
}

static Token make_token(TokenType type, int line, int col) {
    Token tok;
    tok.type = type;
    tok.line = line;
    tok.col = col;
    tok.lexeme = NULL;
    tok.number = 0.0;
    return tok;
}

static TokenType keyword_type(const char *s) {
    if (strcmp(s, "if") == 0) return TOK_IF;
    if (strcmp(s, "else") == 0) return TOK_ELSE;
    if (strcmp(s, "while") == 0) return TOK_WHILE;
    if (strcmp(s, "for") == 0) return TOK_FOR;
    if (strcmp(s, "in") == 0) return TOK_IN;
    if (strcmp(s, "fn") == 0) return TOK_FN;
    if (strcmp(s, "import") == 0) return TOK_IMPORT;
    if (strcmp(s, "from") == 0) return TOK_FROM;
    if (strcmp(s, "mutate") == 0) return TOK_MUTATE;
    if (strcmp(s, "undefine") == 0) return TOK_UNDEFINE;
    if (strcmp(s, "try") == 0) return TOK_TRY;
    if (strcmp(s, "catch") == 0) return TOK_CATCH;
    if (strcmp(s, "finally") == 0) return TOK_FINALLY;
    if (strcmp(s, "throw") == 0) return TOK_THROW;
    if (strcmp(s, "true") == 0) return TOK_TRUE;
    if (strcmp(s, "false") == 0) return TOK_FALSE;
    if (strcmp(s, "null") == 0) return TOK_NULL;
    return TOK_IDENT;
}

static TokenList lex_source(const char *src) {
    Lexer lex;
    lex.src = src;
    lex.len = strlen(src);
    lex.pos = 0;
    lex.line = 1;
    lex.col = 1;
    lex.paren_depth = 0;
    lex.bracket_depth = 0;

    TokenList tokens;
    token_list_init(&tokens);

    while (lex.pos < lex.len) {
        char c = lex.src[lex.pos];
        int line = lex.line;
        int col = lex.col;

        if (c == ' ' || c == '\t' || c == '\r') {
            lex.pos++;
            lex.col++;
            continue;
        }
        if (c == '\n') {
            lex.pos++;
            lex.line++;
            lex.col = 1;
            if (lex.paren_depth == 0 && lex.bracket_depth == 0) {
                token_list_push(&tokens, make_token(TOK_NEWLINE, line, col));
            }
            continue;
        }
        if (c == '#') {
            while (lex.pos < lex.len && lex.src[lex.pos] != '\n') {
                lex.pos++;
                lex.col++;
            }
            continue;
        }
        if (is_ident_start(c)) {
            size_t start = lex.pos;
            while (lex.pos < lex.len && is_ident_part(lex.src[lex.pos])) {
                lex.pos++;
                lex.col++;
            }
            size_t len = lex.pos - start;
            char *ident = xstrndup(lex.src + start, len);
            TokenType type = keyword_type(ident);
            Token tok = make_token(type, line, col);
            tok.lexeme = ident;
            token_list_push(&tokens, tok);
            continue;
        }
        if (isdigit((unsigned char)c) || (c == '.' && lex.pos + 1 < lex.len &&
                                          isdigit((unsigned char)lex.src[lex.pos + 1]))) {
            size_t start = lex.pos;
            bool is_float = false;
            int base = 10;

            if (lex.src[lex.pos] == '0' && lex.pos + 1 < lex.len) {
                char next = lex.src[lex.pos + 1];
                if (next == 'x' || next == 'X') {
                    base = 16;
                    lex.pos += 2;
                    lex.col += 2;
                    start = lex.pos;
                    while (lex.pos < lex.len && digit_value(lex.src[lex.pos]) >= 0 &&
                           digit_value(lex.src[lex.pos]) < base) {
                        lex.pos++;
                        lex.col++;
                    }
                    size_t len = lex.pos - start;
                    if (len == 0) {
                        runtime_fatal("invalid hex literal");
                    }
                    char *num = xstrndup(lex.src + start, len);
                    char *end = NULL;
                    long long val = strtoll(num, &end, base);
                    if (!end || *end != '\0' || val < INT64_MIN || val > INT64_MAX) {
                        runtime_fatal("hex literal overflow");
                    }
                    Token tok = make_token(TOK_INT, line, col);
                    tok.number = (double)val;
                    tok.lexeme = num;
                    token_list_push(&tokens, tok);
                    continue;
                }
                if (next == 'b' || next == 'B') {
                    base = 2;
                    lex.pos += 2;
                    lex.col += 2;
                    start = lex.pos;
                    while (lex.pos < lex.len && digit_value(lex.src[lex.pos]) >= 0 &&
                           digit_value(lex.src[lex.pos]) < base) {
                        lex.pos++;
                        lex.col++;
                    }
                    size_t len = lex.pos - start;
                    if (len == 0) {
                        runtime_fatal("invalid binary literal");
                    }
                    char *num = xstrndup(lex.src + start, len);
                    char *end = NULL;
                    long long val = strtoll(num, &end, base);
                    if (!end || *end != '\0' || val < INT64_MIN || val > INT64_MAX) {
                        runtime_fatal("binary literal overflow");
                    }
                    Token tok = make_token(TOK_INT, line, col);
                    tok.number = (double)val;
                    tok.lexeme = num;
                    token_list_push(&tokens, tok);
                    continue;
                }
                if (next == 'o' || next == 'O') {
                    base = 8;
                    lex.pos += 2;
                    lex.col += 2;
                    start = lex.pos;
                    while (lex.pos < lex.len && digit_value(lex.src[lex.pos]) >= 0 &&
                           digit_value(lex.src[lex.pos]) < base) {
                        lex.pos++;
                        lex.col++;
                    }
                    size_t len = lex.pos - start;
                    if (len == 0) {
                        runtime_fatal("invalid octal literal");
                    }
                    char *num = xstrndup(lex.src + start, len);
                    char *end = NULL;
                    long long val = strtoll(num, &end, base);
                    if (!end || *end != '\0' || val < INT64_MIN || val > INT64_MAX) {
                        runtime_fatal("octal literal overflow");
                    }
                    Token tok = make_token(TOK_INT, line, col);
                    tok.number = (double)val;
                    tok.lexeme = num;
                    token_list_push(&tokens, tok);
                    continue;
                }
            }

            if (lex.src[lex.pos] == '.') {
                is_float = true;
                lex.pos++;
                lex.col++;
                while (lex.pos < lex.len && isdigit((unsigned char)lex.src[lex.pos])) {
                    lex.pos++;
                    lex.col++;
                }
            } else {
                while (lex.pos < lex.len && isdigit((unsigned char)lex.src[lex.pos])) {
                    lex.pos++;
                    lex.col++;
                }
                if (lex.pos < lex.len && lex.src[lex.pos] == '.') {
                    is_float = true;
                    lex.pos++;
                    lex.col++;
                    while (lex.pos < lex.len && isdigit((unsigned char)lex.src[lex.pos])) {
                        lex.pos++;
                        lex.col++;
                    }
                }
            }
            if (lex.pos < lex.len && (lex.src[lex.pos] == 'e' || lex.src[lex.pos] == 'E')) {
                is_float = true;
                lex.pos++;
                lex.col++;
                if (lex.pos < lex.len && (lex.src[lex.pos] == '+' || lex.src[lex.pos] == '-')) {
                    lex.pos++;
                    lex.col++;
                }
                if (lex.pos >= lex.len || !isdigit((unsigned char)lex.src[lex.pos])) {
                    runtime_fatal("invalid float literal");
                }
                while (lex.pos < lex.len && isdigit((unsigned char)lex.src[lex.pos])) {
                    lex.pos++;
                    lex.col++;
                }
            }

            size_t len = lex.pos - start;
            char *num = xstrndup(lex.src + start, len);
            Token tok = make_token(is_float ? TOK_FLOAT : TOK_INT, line, col);
            if (is_float) {
                char *end = NULL;
                double val = strtod(num, &end);
                if (!end || *end != '\0') {
                    runtime_fatal("invalid float literal");
                }
                tok.number = val;
            } else {
                char *end = NULL;
                long long val = strtoll(num, &end, 10);
                if (!end || *end != '\0' || val < INT64_MIN || val > INT64_MAX) {
                    runtime_fatal("int literal overflow");
                }
                tok.number = (double)val;
            }
            tok.lexeme = num;
            token_list_push(&tokens, tok);
            continue;
        }
        if (c == '"') {
            lex.pos++;
            lex.col++;
            size_t cap = 16;
            size_t len = 0;
            char *buf = xmalloc(cap);
            while (lex.pos < lex.len && lex.src[lex.pos] != '"') {
                char ch = lex.src[lex.pos];
                if (ch == '\\' && lex.pos + 1 < lex.len) {
                    char esc = lex.src[lex.pos + 1];
                    switch (esc) {
                    case 'n':
                        ch = '\n';
                        break;
                    case 't':
                        ch = '\t';
                        break;
                    case 'r':
                        ch = '\r';
                        break;
                    case '"':
                        ch = '"';
                        break;
                    case '\\':
                        ch = '\\';
                        break;
                    default:
                        runtime_fatal("invalid escape in string literal");
                    }
                    lex.pos += 2;
                    lex.col += 2;
                } else {
                    lex.pos++;
                    lex.col++;
                }
                if (len + 1 >= cap) {
                    cap *= 2;
                    buf = realloc(buf, cap);
                    if (!buf) {
                        runtime_fatal("out of memory");
                    }
                }
                buf[len++] = ch;
            }
            if (lex.pos < lex.len && lex.src[lex.pos] == '"') {
                lex.pos++;
                lex.col++;
            }
            if (len + 1 >= cap) {
                cap++;
                buf = realloc(buf, cap);
                if (!buf) {
                    runtime_fatal("out of memory");
                }
            }
            buf[len] = '\0';
            Token tok = make_token(TOK_STRING, line, col);
            tok.lexeme = buf;
            token_list_push(&tokens, tok);
            continue;
        }
        switch (c) {
        case '(':
            lex.paren_depth++;
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_LPAREN, line, col));
            break;
        case ')':
            lex.paren_depth--;
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_RPAREN, line, col));
            break;
        case '{':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_LBRACE, line, col));
            break;
        case '}':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_RBRACE, line, col));
            break;
        case '[':
            lex.bracket_depth++;
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_LBRACKET, line, col));
            break;
        case ']':
            lex.bracket_depth--;
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_RBRACKET, line, col));
            break;
        case ',':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_COMMA, line, col));
            break;
        case '.':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_DOT, line, col));
            break;
        case '+':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_PLUS, line, col));
            break;
        case '-':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_MINUS, line, col));
            break;
        case '*':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_STAR, line, col));
            break;
        case '/':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_SLASH, line, col));
            break;
        case '!':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '=') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_NE, line, col));
            } else {
                token_list_push(&tokens, make_token(TOK_NOT, line, col));
            }
            break;
        case '=':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '=') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_EQ, line, col));
            } else {
                token_list_push(&tokens, make_token(TOK_ASSIGN, line, col));
            }
            break;
        case '<':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '=') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_LE, line, col));
            } else {
                token_list_push(&tokens, make_token(TOK_LT, line, col));
            }
            break;
        case '>':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '=') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_GE, line, col));
            } else {
                token_list_push(&tokens, make_token(TOK_GT, line, col));
            }
            break;
        case '&':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '&') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_AND, line, col));
            } else {
                runtime_fatal("unexpected '&'");
            }
            break;
        case '|':
            lex.pos++;
            lex.col++;
            if (lex.src[lex.pos] == '|') {
                lex.pos++;
                lex.col++;
                token_list_push(&tokens, make_token(TOK_OR, line, col));
            } else {
                runtime_fatal("unexpected '|'");
            }
            break;
        case ';':
            lex.pos++;
            lex.col++;
            token_list_push(&tokens, make_token(TOK_NEWLINE, line, col));
            break;
        default:
            runtime_fatal("unexpected character");
        }
    }
    token_list_push(&tokens, make_token(TOK_EOF, lex.line, lex.col));
    return tokens;
}

static Token *peek(Parser *p) {
    return &p->tokens.items[p->current];
}

static Token *previous(Parser *p) {
    return &p->tokens.items[p->current - 1];
}

static bool match(Parser *p, TokenType type) {
    if (peek(p)->type == type) {
        p->current++;
        return true;
    }
    return false;
}

static Token *consume(Parser *p, TokenType type, const char *msg) {
    if (peek(p)->type == type) {
        p->current++;
        return previous(p);
    }
    runtime_fatal(msg);
    return NULL;
}

static void skip_newlines(Parser *p) {
    while (match(p, TOK_NEWLINE)) {
    }
}

static Node *parse_statement(Parser *p);
static Node *parse_expression(Parser *p);
static Node *parse_block(Parser *p);

static Node *parse_if_tail(Parser *p) {
    if (match(p, TOK_IF)) {
        Token *if_tok = previous(p);
        Node *node = node_new(NODE_IF, if_tok->line, if_tok->col);
        node->as.if_expr.cond = parse_expression(p);
        skip_newlines(p);
        node->as.if_expr.then_branch = parse_block(p);
        skip_newlines(p);
        if (match(p, TOK_ELSE)) {
            skip_newlines(p);
            node->as.if_expr.else_branch = parse_if_tail(p);
        } else {
            node->as.if_expr.else_branch = NULL;
        }
        return node;
    }
    return parse_block(p);
}

static Node *parse_block(Parser *p) {
    Token *tok = consume(p, TOK_LBRACE, "expected '{'");
    Node *node = node_new(NODE_BLOCK, tok->line, tok->col);
    node_list_init(&node->as.block.statements);
    skip_newlines(p);
    while (peek(p)->type != TOK_RBRACE && peek(p)->type != TOK_EOF) {
        Node *stmt = parse_statement(p);
        node_list_push(&node->as.block.statements, stmt);
        skip_newlines(p);
    }
    consume(p, TOK_RBRACE, "expected '}'");
    return node;
}

static Node *parse_primary(Parser *p) {
    skip_newlines(p);
    Token *tok = peek(p);
    if (match(p, TOK_INT)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_int((int64_t)tok->number);
        return node;
    }
    if (match(p, TOK_FLOAT)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_float(tok->number);
        return node;
    }
    if (match(p, TOK_STRING)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_string_value(tok->lexeme, strlen(tok->lexeme));
        return node;
    }
    if (match(p, TOK_TRUE)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_bool(true);
        return node;
    }
    if (match(p, TOK_FALSE)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_bool(false);
        return node;
    }
    if (match(p, TOK_NULL)) {
        Node *node = node_new(NODE_LITERAL, tok->line, tok->col);
        node->as.literal = make_null();
        return node;
    }
    if (match(p, TOK_IDENT)) {
        Node *node = node_new(NODE_VAR, tok->line, tok->col);
        node->as.var.name = tok->lexeme;
        return node;
    }
    if (match(p, TOK_LPAREN)) {
        Node *expr = parse_expression(p);
        consume(p, TOK_RPAREN, "expected ')'");
        return expr;
    }
    if (match(p, TOK_FN)) {
        Node *node = node_new(NODE_FN, tok->line, tok->col);
        str_list_init(&node->as.fn.params);
        consume(p, TOK_LPAREN, "expected '('");
        if (!match(p, TOK_RPAREN)) {
            do {
                Token *param = consume(p, TOK_IDENT, "expected parameter name");
                str_list_push(&node->as.fn.params, param->lexeme);
            } while (match(p, TOK_COMMA));
            consume(p, TOK_RPAREN, "expected ')'");
        }
        node->as.fn.body = parse_block(p);
        return node;
    }
    if (match(p, TOK_IF)) {
        Node *node = node_new(NODE_IF, tok->line, tok->col);
        node->as.if_expr.cond = parse_expression(p);
        skip_newlines(p);
        node->as.if_expr.then_branch = parse_block(p);
        skip_newlines(p);
        if (match(p, TOK_ELSE)) {
            skip_newlines(p);
            node->as.if_expr.else_branch = parse_if_tail(p);
        } else {
            node->as.if_expr.else_branch = NULL;
        }
        return node;
    }
    if (match(p, TOK_LBRACKET)) {
        Node *node = node_new(NODE_TABLE, tok->line, tok->col);
        table_literal_init(&node->as.table.items);
        skip_newlines(p);
        if (!match(p, TOK_RBRACKET)) {
            while (true) {
                Token *key = NULL;
                if (match(p, TOK_IDENT)) {
                    key = previous(p);
                } else if (match(p, TOK_STRING)) {
                    key = previous(p);
                } else {
                    runtime_fatal("expected table key");
                }
                consume(p, TOK_ASSIGN, "expected '=' in table literal");
                Node *value = parse_expression(p);
                table_literal_push(&node->as.table.items, key->lexeme, value);
                skip_newlines(p);
                if (match(p, TOK_COMMA)) {
                    skip_newlines(p);
                    continue;
                }
                if (match(p, TOK_RBRACKET)) {
                    break;
                }
                runtime_fatal("expected ',' or ']'");
            }
        }
        return node;
    }
    runtime_fatal("unexpected token");
    return NULL;
}

static Node *parse_postfix(Parser *p) {
    Node *expr = parse_primary(p);
    while (true) {
        if (match(p, TOK_LPAREN)) {
            Node *node = node_new(NODE_CALL, expr->line, expr->col);
            node->as.call.callee = expr;
            node_list_init(&node->as.call.args);
            if (!match(p, TOK_RPAREN)) {
                do {
                    Node *arg = parse_expression(p);
                    node_list_push(&node->as.call.args, arg);
                } while (match(p, TOK_COMMA));
                consume(p, TOK_RPAREN, "expected ')'");
            }
            expr = node;
        } else if (match(p, TOK_DOT)) {
            Token *name = consume(p, TOK_IDENT, "expected property name");
            Node *node = node_new(NODE_DOT, expr->line, expr->col);
            node->as.dot.object = expr;
            node->as.dot.name = name->lexeme;
            expr = node;
        } else if (match(p, TOK_LBRACKET)) {
            Node *node = node_new(NODE_INDEX, expr->line, expr->col);
            node->as.index.object = expr;
            node->as.index.index = parse_expression(p);
            consume(p, TOK_RBRACKET, "expected ']'");
            expr = node;
        } else {
            break;
        }
    }
    return expr;
}

static Node *parse_unary(Parser *p) {
    if (match(p, TOK_NOT) || match(p, TOK_MINUS)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_UNARY, op->line, op->col);
        node->as.unary.op = op->lexeme ? op->lexeme : (op->type == TOK_NOT ? "!" : "-");
        node->as.unary.expr = parse_unary(p);
        return node;
    }
    return parse_postfix(p);
}

static Node *parse_factor(Parser *p) {
    Node *expr = parse_unary(p);
    while (match(p, TOK_STAR) || match(p, TOK_SLASH)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        node->as.binary.op = op->type == TOK_STAR ? "*" : "/";
        node->as.binary.left = expr;
        node->as.binary.right = parse_unary(p);
        expr = node;
    }
    return expr;
}

static Node *parse_term(Parser *p) {
    Node *expr = parse_factor(p);
    while (match(p, TOK_PLUS) || match(p, TOK_MINUS)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        node->as.binary.op = op->type == TOK_PLUS ? "+" : "-";
        node->as.binary.left = expr;
        node->as.binary.right = parse_factor(p);
        expr = node;
    }
    return expr;
}

static Node *parse_comparison(Parser *p) {
    Node *expr = parse_term(p);
    while (match(p, TOK_LT) || match(p, TOK_LE) || match(p, TOK_GT) || match(p, TOK_GE)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        switch (op->type) {
        case TOK_LT:
            node->as.binary.op = "<";
            break;
        case TOK_LE:
            node->as.binary.op = "<=";
            break;
        case TOK_GT:
            node->as.binary.op = ">";
            break;
        case TOK_GE:
            node->as.binary.op = ">=";
            break;
        default:
            break;
        }
        node->as.binary.left = expr;
        node->as.binary.right = parse_term(p);
        expr = node;
    }
    return expr;
}

static Node *parse_equality(Parser *p) {
    Node *expr = parse_comparison(p);
    while (match(p, TOK_EQ) || match(p, TOK_NE)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        node->as.binary.op = op->type == TOK_EQ ? "==" : "!=";
        node->as.binary.left = expr;
        node->as.binary.right = parse_comparison(p);
        expr = node;
    }
    return expr;
}

static Node *parse_and(Parser *p) {
    Node *expr = parse_equality(p);
    while (match(p, TOK_AND)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        node->as.binary.op = "&&";
        node->as.binary.left = expr;
        node->as.binary.right = parse_equality(p);
        expr = node;
    }
    return expr;
}

static Node *parse_or(Parser *p) {
    Node *expr = parse_and(p);
    while (match(p, TOK_OR)) {
        Token *op = previous(p);
        Node *node = node_new(NODE_BINARY, op->line, op->col);
        node->as.binary.op = "||";
        node->as.binary.left = expr;
        node->as.binary.right = parse_and(p);
        expr = node;
    }
    return expr;
}

static Node *parse_assignment(Parser *p) {
    Node *expr = parse_or(p);
    if (match(p, TOK_ASSIGN)) {
        Token *op = previous(p);
        Node *value = parse_assignment(p);
        Node *node = node_new(NODE_ASSIGN, op->line, op->col);
        node->as.assign.target = expr;
        node->as.assign.value = value;
        return node;
    }
    return expr;
}

static Node *parse_expression(Parser *p) {
    skip_newlines(p);
    return parse_assignment(p);
}

static Node *parse_import(Parser *p, Token *tok) {
    Token *name = consume(p, TOK_IDENT, "expected import name");
    consume(p, TOK_FROM, "expected 'from'");
    Token *path = consume(p, TOK_STRING, "expected module path");
    Node *node = node_new(NODE_IMPORT, tok->line, tok->col);
    node->as.import_stmt.name = name->lexeme;
    node->as.import_stmt.path = path->lexeme;
    return node;
}

static Node *parse_try(Parser *p, Token *tok) {
    Node *node = node_new(NODE_TRY, tok->line, tok->col);
    skip_newlines(p);
    node->as.try_expr.try_block = parse_block(p);
    node->as.try_expr.catch_name = NULL;
    node->as.try_expr.catch_any = false;
    node->as.try_expr.catch_block = NULL;
    node->as.try_expr.finally_block = NULL;

    if (match(p, TOK_CATCH)) {
        if (match(p, TOK_STAR)) {
            node->as.try_expr.catch_any = true;
        } else {
            Token *name = consume(p, TOK_IDENT, "expected catch name");
            node->as.try_expr.catch_name = name->lexeme;
        }
        skip_newlines(p);
        node->as.try_expr.catch_block = parse_block(p);
    }
    if (match(p, TOK_FINALLY)) {
        skip_newlines(p);
        node->as.try_expr.finally_block = parse_block(p);
    }
    return node;
}

static Node *parse_statement(Parser *p) {
    skip_newlines(p);
    Token *tok = peek(p);
    if (match(p, TOK_IMPORT)) {
        return parse_import(p, tok);
    }
    if (match(p, TOK_MUTATE)) {
        Node *node = node_new(NODE_MUTATE, tok->line, tok->col);
        node->as.mutate.target = parse_expression(p);
        skip_newlines(p);
        node->as.mutate.body = parse_block(p);
        return node;
    }
    if (match(p, TOK_UNDEFINE)) {
        Node *node = node_new(NODE_UNDEFINE, tok->line, tok->col);
        node->as.undefine.target = parse_expression(p);
        return node;
    }
    if (match(p, TOK_TRY)) {
        return parse_try(p, tok);
    }
    if (match(p, TOK_THROW)) {
        Node *node = node_new(NODE_THROW, tok->line, tok->col);
        node->as.throw_expr.expr = parse_expression(p);
        return node;
    }
    return parse_expression(p);
}

static Node *parse_program(Parser *p) {
    Node *node = node_new(NODE_BLOCK, 1, 1);
    node_list_init(&node->as.block.statements);
    skip_newlines(p);
    while (peek(p)->type != TOK_EOF) {
        Node *stmt = parse_statement(p);
        node_list_push(&node->as.block.statements, stmt);
        skip_newlines(p);
    }
    return node;
}

static EvalResult eval_node(Node *node, Env *env, const char *module_dir);

static FILE *try_open_module(const char *path, const char *module_dir, char *full, size_t full_sz) {
    FILE *f = NULL;
    if (path[0] == '/') {
        snprintf(full, full_sz, "%s", path);
        return fopen(full, "rb");
    }
    if (module_dir) {
        snprintf(full, full_sz, "%s/%s", module_dir, path);
        f = fopen(full, "rb");
        if (f) {
            return f;
        }
        char parent[1024];
        snprintf(parent, sizeof(parent), "%s", module_dir);
        char *slash = strrchr(parent, '/');
        if (slash) {
            *slash = '\0';
        } else {
            snprintf(parent, sizeof(parent), ".");
        }
        snprintf(full, full_sz, "%s/%s", parent, path);
        f = fopen(full, "rb");
        if (f) {
            return f;
        }
    }
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd))) {
        snprintf(full, full_sz, "%s/%s", cwd, path);
        f = fopen(full, "rb");
        if (f) {
            return f;
        }
    }
    snprintf(full, full_sz, "%s", path);
    return fopen(full, "rb");
}

static EvalResult eval_block(Node *node, Env *env, const char *module_dir, bool new_scope) {
    Env *scope = env;
    if (new_scope) {
        scope = env_new(env);
    }
    Value last = make_null();
    for (size_t i = 0; i < node->as.block.statements.count; i++) {
        EvalResult r = eval_node(node->as.block.statements.items[i], scope, module_dir);
        if (r.is_exception) {
            return r;
        }
        last = r.value;
    }
    return ok(last);
}

static EvalResult eval_binary(const char *op, Value left, Value right) {
    if (strcmp(op, "+") == 0) {
        if (left.type == VAL_STRING || right.type == VAL_STRING) {
            return error_msg("string concatenation not implemented");
        }
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '+'");
        }
        if (left.type == VAL_FLOAT || right.type == VAL_FLOAT) {
            return ok(make_float(value_as_double(left) + value_as_double(right)));
        }
        return ok(make_int(left.as.i + right.as.i));
    }
    if (strcmp(op, "-") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '-'");
        }
        if (left.type == VAL_FLOAT || right.type == VAL_FLOAT) {
            return ok(make_float(value_as_double(left) - value_as_double(right)));
        }
        return ok(make_int(left.as.i - right.as.i));
    }
    if (strcmp(op, "*") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '*'");
        }
        if (left.type == VAL_FLOAT || right.type == VAL_FLOAT) {
            return ok(make_float(value_as_double(left) * value_as_double(right)));
        }
        return ok(make_int(left.as.i * right.as.i));
    }
    if (strcmp(op, "/") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '/'");
        }
        if (left.type == VAL_FLOAT || right.type == VAL_FLOAT) {
            return ok(make_float(value_as_double(left) / value_as_double(right)));
        }
        if (right.as.i == 0) {
            return error_msg("division by zero");
        }
        return ok(make_int(left.as.i / right.as.i));
    }
    if (strcmp(op, "==") == 0) {
        return ok(make_bool(value_equal(left, right)));
    }
    if (strcmp(op, "!=") == 0) {
        return ok(make_bool(!value_equal(left, right)));
    }
    if (strcmp(op, "<") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '<'");
        }
        return ok(make_bool(value_as_double(left) < value_as_double(right)));
    }
    if (strcmp(op, "<=") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '<='");
        }
        return ok(make_bool(value_as_double(left) <= value_as_double(right)));
    }
    if (strcmp(op, ">") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '>'");
        }
        return ok(make_bool(value_as_double(left) > value_as_double(right)));
    }
    if (strcmp(op, ">=") == 0) {
        if (!value_is_number(left) || !value_is_number(right)) {
            return error_msg("non-numeric '>='");
        }
        return ok(make_bool(value_as_double(left) >= value_as_double(right)));
    }
    if (strcmp(op, "&&") == 0) {
        return ok(make_bool(value_is_truthy(left) && value_is_truthy(right)));
    }
    if (strcmp(op, "||") == 0) {
        return ok(make_bool(value_is_truthy(left) || value_is_truthy(right)));
    }
    return error_msg("unknown binary op");
}

static EvalResult eval_call(Value callee, int argc, Value *argv, const char *module_dir) {
    if (callee.type != VAL_FUNCTION) {
        return error_msg("call on non-function");
    }
    Function *fn = callee.as.fn;
    if (fn->is_native) {
        EvalResult err;
        err.is_exception = false;
        err.value = make_null();
        Value out = fn->native(argc, argv, &err);
        if (err.is_exception) {
            return err;
        }
        return ok(out);
    }
    if (argc != fn->arity) {
        return error_msg("arity mismatch");
    }
    Env *call_env = env_new(fn->env);
    for (int i = 0; i < fn->arity; i++) {
        env_define(call_env, fn->params.items[i], argv[i]);
    }
    return eval_block(fn->body, call_env, module_dir, true);
}

EvalResult call_function(Value callee, int argc, Value *argv, const char *module_dir) {
    return eval_call(callee, argc, argv, module_dir);
}

static EvalResult eval_undefine(Node *node, Env *env, const char *module_dir) {
    Node *target = node->as.undefine.target;
    if (target->type != NODE_DOT && target->type != NODE_INDEX) {
        return error_msg("undefine target must be slot");
    }
    if (target->type == NODE_DOT) {
        EvalResult obj_r = eval_node(target->as.dot.object, env, module_dir);
        if (obj_r.is_exception) {
            return obj_r;
        }
        if (obj_r.value.type != VAL_TABLE) {
            return error_msg("undefine on non-table");
        }
        if (strcmp(target->as.dot.name, "proto") == 0) {
            return error_msg("cannot undefine proto");
        }
        Value key = make_string_value(target->as.dot.name, strlen(target->as.dot.name));
        if (!table_delete(obj_r.value.as.table, key)) {
            return error_msg("object is frozen");
        }
        return ok(make_null());
    }
    EvalResult obj_r = eval_node(target->as.index.object, env, module_dir);
    if (obj_r.is_exception) {
        return obj_r;
    }
    EvalResult idx_r = eval_node(target->as.index.index, env, module_dir);
    if (idx_r.is_exception) {
        return idx_r;
    }
    if (obj_r.value.type != VAL_TABLE) {
        return error_msg("undefine on non-table");
    }
    if (idx_r.value.type == VAL_STRING &&
        strcmp(idx_r.value.as.str->data, "proto") == 0) {
        return error_msg("cannot undefine proto");
    }
    if (!table_delete(obj_r.value.as.table, idx_r.value)) {
        return error_msg("object is frozen");
    }
    return ok(make_null());
}

static EvalResult eval_node(Node *node, Env *env, const char *module_dir) {
    switch (node->type) {
    case NODE_LITERAL:
        return ok(node->as.literal);
    case NODE_VAR: {
        Value out;
        if (!env_get(env, node->as.var.name, &out)) {
            return error_msg("undefined variable");
        }
        return ok(out);
    }
    case NODE_ASSIGN: {
        EvalResult value_r = eval_node(node->as.assign.value, env, module_dir);
        if (value_r.is_exception) {
            return value_r;
        }
        if (value_r.value.type == VAL_UNDEFINED) {
            return error_msg("cannot assign undefined");
        }
        Node *target = node->as.assign.target;
        if (target->type == NODE_VAR) {
            env_assign(env, target->as.var.name, value_r.value);
            return ok(value_r.value);
        }
        if (target->type == NODE_DOT) {
            EvalResult obj_r = eval_node(target->as.dot.object, env, module_dir);
            if (obj_r.is_exception) {
                return obj_r;
            }
            if (obj_r.value.type != VAL_TABLE) {
                return error_msg("assignment on non-table");
            }
            if (strcmp(target->as.dot.name, "proto") == 0) {
                if (!table_set_proto(obj_r.value.as.table, value_r.value)) {
                    return error_msg("invalid proto");
                }
                return ok(value_r.value);
            }
            Value key = make_string_value(target->as.dot.name, strlen(target->as.dot.name));
            if (!table_set(obj_r.value.as.table, key, value_r.value)) {
                return error_msg("object is frozen");
            }
            return ok(value_r.value);
        }
        if (target->type == NODE_INDEX) {
            EvalResult obj_r = eval_node(target->as.index.object, env, module_dir);
            if (obj_r.is_exception) {
                return obj_r;
            }
            EvalResult idx_r = eval_node(target->as.index.index, env, module_dir);
            if (idx_r.is_exception) {
                return idx_r;
            }
            if (obj_r.value.type != VAL_TABLE) {
                return error_msg("assignment on non-table");
            }
            if (idx_r.value.type == VAL_STRING &&
                strcmp(idx_r.value.as.str->data, "proto") == 0) {
                if (!table_set_proto(obj_r.value.as.table, value_r.value)) {
                    return error_msg("invalid proto");
                }
                return ok(value_r.value);
            }
            if (!table_set(obj_r.value.as.table, idx_r.value, value_r.value)) {
                return error_msg("object is frozen");
            }
            return ok(value_r.value);
        }
        return error_msg("invalid assignment");
    }
    case NODE_BINARY: {
        EvalResult left_r = eval_node(node->as.binary.left, env, module_dir);
        if (left_r.is_exception) {
            return left_r;
        }
        EvalResult right_r = eval_node(node->as.binary.right, env, module_dir);
        if (right_r.is_exception) {
            return right_r;
        }
        return eval_binary(node->as.binary.op, left_r.value, right_r.value);
    }
    case NODE_UNARY: {
        EvalResult inner = eval_node(node->as.unary.expr, env, module_dir);
        if (inner.is_exception) {
            return inner;
        }
        if (strcmp(node->as.unary.op, "!") == 0) {
            return ok(make_bool(!value_is_truthy(inner.value)));
        }
        if (strcmp(node->as.unary.op, "-") == 0) {
            if (inner.value.type == VAL_FLOAT) {
                return ok(make_float(-inner.value.as.f));
            }
            return ok(make_int(-inner.value.as.i));
        }
        return error_msg("unknown unary op");
    }
    case NODE_CALL: {
        EvalResult callee_r = eval_node(node->as.call.callee, env, module_dir);
        if (callee_r.is_exception) {
            return callee_r;
        }
        size_t argc = node->as.call.args.count;
        Value *argv = xmalloc(sizeof(Value) * argc);
        for (size_t i = 0; i < argc; i++) {
            EvalResult arg_r = eval_node(node->as.call.args.items[i], env, module_dir);
            if (arg_r.is_exception) {
                free(argv);
                return arg_r;
            }
            argv[i] = arg_r.value;
        }
        EvalResult res = eval_call(callee_r.value, (int)argc, argv, module_dir);
        free(argv);
        return res;
    }
    case NODE_DOT: {
        EvalResult obj_r = eval_node(node->as.dot.object, env, module_dir);
        if (obj_r.is_exception) {
            return obj_r;
        }
        if (obj_r.value.type != VAL_TABLE) {
            return error_msg("lookup on non-table");
        }
        Value key = make_string_value(node->as.dot.name, strlen(node->as.dot.name));
        return ok(table_get(obj_r.value.as.table, key));
    }
    case NODE_INDEX: {
        EvalResult obj_r = eval_node(node->as.index.object, env, module_dir);
        if (obj_r.is_exception) {
            return obj_r;
        }
        EvalResult idx_r = eval_node(node->as.index.index, env, module_dir);
        if (idx_r.is_exception) {
            return idx_r;
        }
        if (obj_r.value.type != VAL_TABLE) {
            return error_msg("index on non-table");
        }
        return ok(table_get(obj_r.value.as.table, idx_r.value));
    }
    case NODE_IF: {
        EvalResult cond = eval_node(node->as.if_expr.cond, env, module_dir);
        if (cond.is_exception) {
            return cond;
        }
        if (value_is_truthy(cond.value)) {
            return eval_block(node->as.if_expr.then_branch, env, module_dir, true);
        }
        if (node->as.if_expr.else_branch) {
            return eval_block(node->as.if_expr.else_branch, env, module_dir, true);
        }
        return ok(make_null());
    }
    case NODE_FN: {
        Function *fn = xmalloc(sizeof(Function));
        fn->is_native = false;
        fn->arity = (int)node->as.fn.params.count;
        fn->params = node->as.fn.params;
        fn->body = node->as.fn.body;
        fn->env = env;
        fn->native = NULL;
        return ok(make_function(fn));
    }
    case NODE_IMPORT: {
        const char *path = node->as.import_stmt.path;
        Value lib;
        if (runtime_import(path, env, &lib)) {
            env_define(env, node->as.import_stmt.name, lib);
            return ok(lib);
        }
        char full[1024];
        FILE *f = try_open_module(path, module_dir, full, sizeof(full));
        if (!f) {
            return error_msg("cannot open module");
        }
        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *src = xmalloc(len + 1);
        fread(src, 1, len, f);
        src[len] = '\0';
        fclose(f);

        TokenList tokens = lex_source(src);
        Parser parser;
        parser.tokens = tokens;
        parser.current = 0;
        set_parse_context(&parser, full);
        Node *program = parse_program(&parser);
        clear_parse_context();

        char *dir = NULL;
        char *slash = strrchr(full, '/');
        if (slash) {
            dir = xstrndup(full, (size_t)(slash - full));
        }
        Env *mod_env = env_new(env);
        EvalResult res = eval_block(program, mod_env, dir, false);
        if (dir) {
            free(dir);
        }
        if (res.is_exception) {
            return res;
        }
        env_define(env, node->as.import_stmt.name, res.value);
        return ok(res.value);
    }
    case NODE_MUTATE: {
        EvalResult target = eval_node(node->as.mutate.target, env, module_dir);
        if (target.is_exception) {
            return target;
        }
        if (target.value.type != VAL_TABLE) {
            return error_msg("mutate on non-table");
        }
        Table *t = target.value.as.table;
        table_adjust_thaw(t, 1);
        EvalResult res = eval_block(node->as.mutate.body, env, module_dir, true);
        table_adjust_thaw(t, -1);
        if (res.is_exception) {
            return res;
        }
        return res;
    }
    case NODE_UNDEFINE:
        return eval_undefine(node, env, module_dir);
    case NODE_TRY: {
        EvalResult attempt = eval_block(node->as.try_expr.try_block, env, module_dir, true);
        EvalResult handled = attempt;
        if (attempt.is_exception && node->as.try_expr.catch_block) {
            if (node->as.try_expr.catch_any) {
                handled = eval_block(node->as.try_expr.catch_block, env, module_dir, true);
            } else {
                Env *catch_env = env_new(env);
                env_define(catch_env, node->as.try_expr.catch_name, attempt.value);
                handled = eval_block(node->as.try_expr.catch_block, catch_env, module_dir, true);
            }
        }
        if (node->as.try_expr.finally_block) {
            EvalResult fin = eval_block(node->as.try_expr.finally_block, env, module_dir, true);
            if (fin.is_exception) {
                runtime_fatal("finally cannot throw");
            }
        }
        return handled;
    }
    case NODE_THROW: {
        EvalResult val = eval_node(node->as.throw_expr.expr, env, module_dir);
        if (val.is_exception) {
            return val;
        }
        return exception(val.value);
    }
    case NODE_TABLE: {
        Table *t = table_new();
        for (size_t i = 0; i < node->as.table.items.count; i++) {
            char *key = node->as.table.items.keys[i];
            EvalResult val = eval_node(node->as.table.items.values[i], env, module_dir);
            if (val.is_exception) {
                return val;
            }
            if (strcmp(key, "proto") == 0) {
                if (!table_set_proto(t, val.value)) {
                    return error_msg("invalid proto");
                }
            } else {
                Value k = make_string_value(key, strlen(key));
                if (!table_set(t, k, val.value)) {
                    return error_msg("object is frozen");
                }
            }
        }
        return ok(make_table(t));
    }
    case NODE_BLOCK:
        return eval_block(node, env, module_dir, false);
    }
    return error_msg("unknown node");
}

static Value native_clone(int argc, Value *argv, EvalResult *err) {
    (void)err;
    if (argc != 1) {
        return make_null();
    }
    Table *t = table_new();
    if (!table_set_proto(t, argv[0])) {
        runtime_fatal("invalid proto in clone");
    }
    return make_table(t);
}

static Value native_has(int argc, Value *argv, EvalResult *err) {
    (void)err;
    if (argc != 2 || argv[0].type != VAL_TABLE) {
        return make_bool(false);
    }
    return make_bool(table_has_local(argv[0].as.table, argv[1]));
}

static Value native_freeze(int argc, Value *argv, EvalResult *err) {
    (void)err;
    if (argc == 1 && argv[0].type == VAL_TABLE) {
        argv[0].as.table->frozen = true;
    }
    return make_null();
}

static Value native_is_absent(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isAbsent expects (value)", 23);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_UNDEFINED);
}

static Value native_is_int(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isInt expects (value)", 22);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_INT);
}

static Value native_is_float(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isFloat expects (value)", 24);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_FLOAT);
}

static Value native_is_string(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isString expects (value)", 25);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_STRING);
}

static Value native_is_bool(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isBool expects (value)", 23);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_BOOL);
}

static Value native_is_table(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isTable expects (value)", 24);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_TABLE);
}

static Value native_is_function(int argc, Value *argv, EvalResult *err) {
    if (argc != 1) {
        err->is_exception = true;
        err->value = make_string_value("isFunction expects (value)", 27);
        return make_null();
    }
    return make_bool(argv[0].type == VAL_FUNCTION);
}

static Env *make_root_env(void) {
    Env *env = env_new(NULL);
    Function *clone_fn = xmalloc(sizeof(Function));
    clone_fn->is_native = true;
    clone_fn->native = native_clone;
    env_define(env, "clone", make_function(clone_fn));

    Function *has_fn = xmalloc(sizeof(Function));
    has_fn->is_native = true;
    has_fn->native = native_has;
    env_define(env, "has", make_function(has_fn));

    Function *freeze_fn = xmalloc(sizeof(Function));
    freeze_fn->is_native = true;
    freeze_fn->native = native_freeze;
    env_define(env, "freeze", make_function(freeze_fn));

    Function *is_absent_fn = xmalloc(sizeof(Function));
    is_absent_fn->is_native = true;
    is_absent_fn->native = native_is_absent;
    env_define(env, "isAbsent", make_function(is_absent_fn));

    Function *is_int_fn = xmalloc(sizeof(Function));
    is_int_fn->is_native = true;
    is_int_fn->native = native_is_int;
    env_define(env, "isInt", make_function(is_int_fn));

    Function *is_float_fn = xmalloc(sizeof(Function));
    is_float_fn->is_native = true;
    is_float_fn->native = native_is_float;
    env_define(env, "isFloat", make_function(is_float_fn));

    Function *is_string_fn = xmalloc(sizeof(Function));
    is_string_fn->is_native = true;
    is_string_fn->native = native_is_string;
    env_define(env, "isString", make_function(is_string_fn));

    Function *is_bool_fn = xmalloc(sizeof(Function));
    is_bool_fn->is_native = true;
    is_bool_fn->native = native_is_bool;
    env_define(env, "isBool", make_function(is_bool_fn));

    Function *is_table_fn = xmalloc(sizeof(Function));
    is_table_fn->is_native = true;
    is_table_fn->native = native_is_table;
    env_define(env, "isTable", make_function(is_table_fn));

    Function *is_function_fn = xmalloc(sizeof(Function));
    is_function_fn->is_native = true;
    is_function_fn->native = native_is_function;
    env_define(env, "isFunction", make_function(is_function_fn));

    return env;
}

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = xmalloc(len + 1);
    fread(src, 1, len, f);
    src[len] = '\0';
    fclose(f);
    return src;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <file.plx>\n", argv[0]);
        return 1;
    }
    char *src = read_file(argv[1]);
    if (!src) {
        fprintf(stderr, "cannot open %s\n", argv[1]);
        return 1;
    }
    TokenList tokens = lex_source(src);
    Parser parser;
    parser.tokens = tokens;
    parser.current = 0;
    set_parse_context(&parser, argv[1]);
    Node *program = parse_program(&parser);
    clear_parse_context();
    Env *env = make_root_env();
    char *dir = NULL;
    char *slash = strrchr(argv[1], '/');
    if (slash) {
        dir = xstrndup(argv[1], (size_t)(slash - argv[1]));
    }
    runtime_init(argc, argv, dir);
    EvalResult res = eval_block(program, env, dir, false);
    if (res.is_exception) {
        fprintf(stderr, "uncaught exception: ");
        print_value(res.value);
        fprintf(stderr, "\n");
        return 1;
    }
    if (dir) {
        free(dir);
    }
    return 0;
}
