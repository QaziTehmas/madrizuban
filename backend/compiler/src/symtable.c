/*
 * MadriZuban Compiler v1.0
 * symtable.c - Symbol Table (hash-chained)
 *
 * Stores all declared variables with their type and value.
 */

#include "madrizuban.h"

/* ─── Simple hash ─────────────────────────────────────────────────────────── */
static unsigned int hash_name(const char *name) {
    unsigned int h = 5381;
    while (*name)
        h = ((h << 5) + h) ^ (unsigned char)(*name++);
    return h % SYM_TABLE_SIZE;
}

/* ─── Create ──────────────────────────────────────────────────────────────── */
SymTable *symtable_create(void) {
    SymTable *st = (SymTable *)calloc(1, sizeof(SymTable));
    if (!st) {
        fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
                " bhaiyaaaa Memory nahi mili symbol table ke liye!\n");
        exit(1);
    }
    return st;
}

/* ─── Destroy ─────────────────────────────────────────────────────────────── */
void symtable_destroy(SymTable *st) {
    if (!st) return;
    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        SymEntry *e = st->buckets[i];
        while (e) {
            SymEntry *tmp = e->next;
            free(e);
            e = tmp;
        }
    }
    free(st);
}

/* ─── Lookup ──────────────────────────────────────────────────────────────── */
SymEntry *symtable_lookup(SymTable *st, const char *name) {
    unsigned int h = hash_name(name);
    SymEntry *e = st->buckets[h];
    while (e) {
        if (strcmp(e->name, name) == 0) return e;
        e = e->next;
    }
    return NULL;
}

/* ─── Insert ──────────────────────────────────────────────────────────────── */
int symtable_insert(SymTable *st, const char *name, DataType dtype, int line) {
    if (symtable_lookup(st, name)) {
        fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
                " bhaiyaaaa Line %d: Yeh variable pehle se mojood hai → '%s'\n",
                line, name);
        return 0; /* failure */
    }
    SymEntry *e = (SymEntry *)calloc(1, sizeof(SymEntry));
    strncpy(e->name, name, 63);
    e->dtype         = dtype;
    e->initialized   = 0;
    e->line_declared = line;
    unsigned int h   = hash_name(name);
    e->next          = st->buckets[h];
    st->buckets[h]   = e;
    st->count++;
    return 1; /* success */
}

/* ─── Print (debug) ───────────────────────────────────────────────────────── */
void symtable_print(SymTable *st) {
    printf(CYAN "\n=== MadriZuban Symbol Table (%d entries) ===\n" RESET, st->count);
    printf("%-20s %-10s %-12s %s\n", "Variable", "Type", "Initialized", "Declared Line");
    printf("%-20s %-10s %-12s %s\n", "--------", "----", "-----------", "-------------");
    for (int i = 0; i < SYM_TABLE_SIZE; i++) {
        SymEntry *e = st->buckets[i];
        while (e) {
            printf("%-20s %-10s %-12s %d\n",
                   e->name,
                   dtype_str(e->dtype),
                   e->initialized ? "haan" : "nahi",
                   e->line_declared);
            e = e->next;
        }
    }
    printf("\n");
}

/* ─── dtype_str (also used by semantic.c) ────────────────────────────────── */
const char *dtype_str(DataType t) {
    switch (t) {
        case TYPE_INT:    return "int";
        case TYPE_STRING: return "string";
        case TYPE_BOOL:   return "bool";
        case TYPE_VOID:   return "void";
        default:          return "error";
    }
}
