#ifndef SYMTABLE_H
#define SYMTABLE_H

#define MAX_SYMBOLS 256
#define MAX_NAME_LEN 64

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOL
} DataType;

typedef union {
    int   int_val;
    char  str_val[256];
    int   bool_val;   /* 1 = sach, 0 = jhoot */
} Value;

typedef struct {
    char     name[MAX_NAME_LEN];
    DataType type;
    Value    value;
    int      initialized;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int    count;
} SymTable;

void   sym_init(SymTable *t);
int    sym_declare(SymTable *t, const char *name, DataType type);
Symbol *sym_lookup(SymTable *t, const char *name);
void   sym_set_int(SymTable *t, const char *name, int val);
void   sym_set_str(SymTable *t, const char *name, const char *val);
void   sym_set_bool(SymTable *t, const char *name, int val);

#endif
