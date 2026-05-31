#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

void sym_init(SymTable *t) {
    t->count = 0;
    memset(t->symbols, 0, sizeof(t->symbols));
}

int sym_declare(SymTable *t, const char *name, DataType type) {
    /* Check for re-declaration */
    for (int i = 0; i < t->count; i++) {
        if (strcmp(t->symbols[i].name, name) == 0)
            return -1; /* already exists */
    }
    if (t->count >= MAX_SYMBOLS) return -2;
    strncpy(t->symbols[t->count].name, name, MAX_NAME_LEN - 1);
    t->symbols[t->count].type = type;
    t->symbols[t->count].initialized = 0;
    t->count++;
    return 0;
}

Symbol *sym_lookup(SymTable *t, const char *name) {
    for (int i = 0; i < t->count; i++) {
        if (strcmp(t->symbols[i].name, name) == 0)
            return &t->symbols[i];
    }
    return NULL;
}

void sym_set_int(SymTable *t, const char *name, int val) {
    Symbol *s = sym_lookup(t, name);
    if (s) { s->value.int_val = val; s->initialized = 1; }
}

void sym_set_str(SymTable *t, const char *name, const char *val) {
    Symbol *s = sym_lookup(t, name);
    if (s) { strncpy(s->value.str_val, val, 255); s->initialized = 1; }
}

void sym_set_bool(SymTable *t, const char *name, int val) {
    Symbol *s = sym_lookup(t, name);
    if (s) { s->value.bool_val = val; s->initialized = 1; }
}
