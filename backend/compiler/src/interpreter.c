/*
 * MadriZuban Compiler v1.0
 * interpreter.c - Tree-Walk Interpreter
 *
 * Executes the AST directly, producing real program output.
 * Handles: int/string/bool, arithmetic, comparisons,
 *          agar/warna, jabtak, arzkro.
 */

#include "madrizuban.h"

/* ─── Runtime value ───────────────────────────────────────────────────────── */
typedef enum { RVAL_INT, RVAL_STRING, RVAL_BOOL, RVAL_ERR } RValType;

typedef struct {
    RValType type;
    int      ival;
    char     sval[256];
    int      bval;
} RVal;

static RVal make_int(int v)         { RVal r; r.type=RVAL_INT; r.ival=v; r.sval[0]='\0'; r.bval=0; return r; }
static RVal make_str(const char *s) { RVal r; r.type=RVAL_STRING; r.ival=0; strncpy(r.sval,s,255); r.bval=0; return r; }
static RVal make_bool(int v)        { RVal r; r.type=RVAL_BOOL; r.ival=0; r.sval[0]='\0'; r.bval=v; return r; }
static RVal make_err(void)          { RVal r; r.type=RVAL_ERR; r.ival=0; r.sval[0]='\0'; r.bval=0; return r; }

static int rval_truthy(RVal v) {
    if (v.type == RVAL_INT)  return v.ival != 0;
    if (v.type == RVAL_BOOL) return v.bval;
    if (v.type == RVAL_STRING) return strlen(v.sval) > 0;
    return 0;
}

/* ─── Forward ─────────────────────────────────────────────────────────────── */
static RVal eval_expr(ASTNode *node, SymTable *st);
static void exec_stmt(ASTNode *node, SymTable *st);

/* ─── Evaluate expression ─────────────────────────────────────────────────── */
static RVal eval_expr(ASTNode *node, SymTable *st) {
    if (!node) return make_err();

    switch (node->type) {

        case NODE_INT_LIT:
            return make_int(node->ival);

        case NODE_STRING_LIT:
            return make_str(node->sval);

        case NODE_BOOL_LIT:
            return make_bool(node->bval);

        case NODE_IDENT: {
            SymEntry *e = symtable_lookup(st, node->sval);
            if (!e) {
                fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
                        " bhaiyaaaa Runtime: '%s' nahi mila\n", node->sval);
                return make_err();
            }
            if (e->dtype == TYPE_INT)    return make_int(e->ival);
            if (e->dtype == TYPE_STRING) return make_str(e->sval);
            if (e->dtype == TYPE_BOOL)   return make_bool(e->bval);
            return make_err();
        }

        case NODE_BINOP: {
            RVal L = eval_expr(node->left,  st);
            RVal R = eval_expr(node->right, st);

            /* Comparisons */
            if (strcmp(node->op, "==") == 0) {
                if (L.type == RVAL_INT)    return make_bool(L.ival  == R.ival);
                if (L.type == RVAL_BOOL)   return make_bool(L.bval  == R.bval);
                if (L.type == RVAL_STRING) return make_bool(strcmp(L.sval, R.sval) == 0);
            }
            if (strcmp(node->op, "!=") == 0) {
                if (L.type == RVAL_INT)    return make_bool(L.ival  != R.ival);
                if (L.type == RVAL_BOOL)   return make_bool(L.bval  != R.bval);
                if (L.type == RVAL_STRING) return make_bool(strcmp(L.sval, R.sval) != 0);
            }
            if (strcmp(node->op, "<")  == 0) return make_bool(L.ival <  R.ival);
            if (strcmp(node->op, ">")  == 0) return make_bool(L.ival >  R.ival);
            if (strcmp(node->op, "<=") == 0) return make_bool(L.ival <= R.ival);
            if (strcmp(node->op, ">=") == 0) return make_bool(L.ival >= R.ival);

            /* Arithmetic */
            if (strcmp(node->op, "+") == 0) return make_int(L.ival + R.ival);
            if (strcmp(node->op, "-") == 0) return make_int(L.ival - R.ival);
            if (strcmp(node->op, "*") == 0) return make_int(L.ival * R.ival);
            if (strcmp(node->op, "/") == 0) {
                if (R.ival == 0) {
                    fprintf(stderr, YELLOW "[MadriZuban Khabardar]" RESET
                            " bhaiyaaaa Zero se taqseem? Serious ho tum?\n");
                    return make_int(0);
                }
                return make_int(L.ival / R.ival);
            }

            return make_err();
        }

        default:
            return make_err();
    }
}

/* ─── Execute statement ───────────────────────────────────────────────────── */
static void exec_stmt(ASTNode *node, SymTable *st) {
    if (!node) return;

    switch (node->type) {

        case NODE_PROGRAM:
        case NODE_BLOCK: {
            ASTNode *s = node->body;
            while (s) { exec_stmt(s, st); s = s->next; }
            break;
        }

        case NODE_VAR_DECL: {
            RVal val = eval_expr(node->left, st);
            SymEntry *e = symtable_lookup(st, node->sval);
            if (!e) {
                /* Insert at runtime if not already there (second pass) */
                symtable_insert(st, node->sval, node->dtype, node->line);
                e = symtable_lookup(st, node->sval);
            }
            if (e) {
                e->initialized = 1;
                if (val.type == RVAL_INT)    { e->ival = val.ival; }
                if (val.type == RVAL_STRING) { strncpy(e->sval, val.sval, 255); }
                if (val.type == RVAL_BOOL)   { e->bval = val.bval; }
            }
            break;
        }

        case NODE_ASSIGN: {
            RVal val = eval_expr(node->left, st);
            SymEntry *e = symtable_lookup(st, node->sval);
            if (e) {
                e->initialized = 1;
                if (val.type == RVAL_INT)    { e->ival = val.ival; }
                if (val.type == RVAL_STRING) { strncpy(e->sval, val.sval, 255); }
                if (val.type == RVAL_BOOL)   { e->bval = val.bval; }
            }
            break;
        }

        case NODE_ARZKRO: {
            RVal val = eval_expr(node->left, st);
            if (val.type == RVAL_INT)    printf("%d\n", val.ival);
            if (val.type == RVAL_STRING) printf("%s\n", val.sval);
            if (val.type == RVAL_BOOL)   printf("%s\n", val.bval ? "sach" : "jhoot");
            break;
        }

        case NODE_AGAR: {
            RVal cond = eval_expr(node->cond, st);
            if (rval_truthy(cond))
                exec_stmt(node->body, st);
            else if (node->elseb)
                exec_stmt(node->elseb, st);
            break;
        }

        case NODE_JABTAK: {
            int iter = 0;
            while (rval_truthy(eval_expr(node->cond, st))) {
                exec_stmt(node->body, st);
                if (++iter > 100000) {
                    fprintf(stderr, YELLOW "[MadriZuban Khabardar]" RESET
                            " bhaiyaaaa Infinite loop lag raha hai — rok diya\n");
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
}

/* ─── Public entry ────────────────────────────────────────────────────────── */
void interpreter_run(ASTNode *root, SymTable *st) {
    exec_stmt(root, st);
}
