/*
 * MadriZuban Compiler v1.0
 * semantic.c - Semantic Analysis
 *
 * Checks:
 *   1. Variable declared before use
 *   2. No re-declaration of same variable
 *   3. Type compatibility in assignments and expressions
 *   4. arzkro used on valid expression
 *   5. Division by zero (constant)
 *   6. bool conditions in agar/jabtak
 */

#include "madrizuban.h"

static int sem_errors   = 0;
static int sem_warnings = 0;

/* ─── Forward ─────────────────────────────────────────────────────────────── */
static DataType check_expr(ASTNode *node, SymTable *st);
static void     check_stmt(ASTNode *node, SymTable *st);

/* ─── Error/Warning helpers ───────────────────────────────────────────────── */
static void sem_error(int line, const char *msg, const char *extra) {
    fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
            " bhaiyaaaa Line %d: %s%s%s\n",
            line, msg,
            extra ? " → '" : "",
            extra ? extra  : "");
    if (extra) fprintf(stderr, "'\n");
    sem_errors++;
}

static void sem_warn(int line, const char *msg) {
    fprintf(stderr, YELLOW "[MadriZuban Khabardar]" RESET
            " bhaiyaaaa Line %d: %s\n", line, msg);
    sem_warnings++;
}

/* ─── Type-check expression, returns inferred type ───────────────────────── */
static DataType check_expr(ASTNode *node, SymTable *st) {
    if (!node) return TYPE_ERROR;

    switch (node->type) {

        case NODE_INT_LIT:
            return TYPE_INT;

        case NODE_STRING_LIT:
            return TYPE_STRING;

        case NODE_BOOL_LIT:
            return TYPE_BOOL;

        case NODE_IDENT: {
            SymEntry *e = symtable_lookup(st, node->sval);
            if (!e) {
                sem_error(node->line,
                          "Yeh variable kabhi bataya nahi", node->sval);
                return TYPE_ERROR;
            }
            if (!e->initialized) {
                char buf[128];
                snprintf(buf, sizeof(buf),
                         "'%s' use kiya lekin initialize nahi tha", node->sval);
                sem_warn(node->line, buf);
            }
            return e->dtype;
        }

        case NODE_BINOP: {
            DataType lt = check_expr(node->left,  st);
            DataType rt = check_expr(node->right, st);

            /* Division by zero check (constant folding) */
            if ((strcmp(node->op, "/") == 0) &&
                node->right && node->right->type == NODE_INT_LIT &&
                node->right->ival == 0) {
                sem_warn(node->line,
                         "Zero se taqseem? Serious ho tum?");
            }

            /* Comparison operators → bool result */
            if (strcmp(node->op, "==") == 0 || strcmp(node->op, "!=") == 0 ||
                strcmp(node->op, "<")  == 0 || strcmp(node->op, ">")  == 0 ||
                strcmp(node->op, "<=") == 0 || strcmp(node->op, ">=") == 0) {
                if (lt != rt && lt != TYPE_ERROR && rt != TYPE_ERROR) {
                    sem_error(node->line,
                              "Comparison mein alag types nahi chal sakte", NULL);
                }
                return TYPE_BOOL;
            }

            /* Arithmetic → int only */
            if (lt == TYPE_STRING || rt == TYPE_STRING) {
                sem_error(node->line,
                          "Arithmetic string par nahi hoti bhai", NULL);
                return TYPE_ERROR;
            }
            if (lt != TYPE_INT || rt != TYPE_INT) {
                sem_error(node->line,
                          "Arithmetic sirf int values ke liye hai", NULL);
                return TYPE_ERROR;
            }
            return TYPE_INT;
        }

        default:
            return TYPE_ERROR;
    }
}

/* ─── Type-check statement ────────────────────────────────────────────────── */
static void check_stmt(ASTNode *node, SymTable *st) {
    if (!node) return;

    switch (node->type) {

        case NODE_PROGRAM:
        case NODE_BLOCK: {
            ASTNode *s = node->body;
            while (s) {
                check_stmt(s, st);
                s = s->next;
            }
            break;
        }

        case NODE_VAR_DECL: {
            /* Insert into symbol table (will print error if duplicate) */
            int ok = symtable_insert(st, node->sval, node->dtype, node->line);
            if (ok && node->left) {
                DataType rhs = check_expr(node->left, st);
                SymEntry *e  = symtable_lookup(st, node->sval);
                /* Type compatibility check */
                if (rhs != TYPE_ERROR && rhs != node->dtype) {
                    char buf[200];
                    snprintf(buf, sizeof(buf),
                             "'%s' ko %s type dene ki koshish, lekin value %s hai",
                             node->sval, dtype_str(node->dtype), dtype_str(rhs));
                    sem_error(node->line, buf, NULL);
                } else if (e) {
                    e->initialized = 1;
                    /* Store initial values for bool */
                    if (node->dtype == TYPE_BOOL && node->left->type == NODE_BOOL_LIT)
                        e->bval = node->left->bval;
                }
            }
            break;
        }

        case NODE_ASSIGN: {
            SymEntry *e = symtable_lookup(st, node->sval);
            if (!e) {
                sem_error(node->line,
                          "Yeh variable kabhi bataya nahi", node->sval);
            } else {
                DataType rhs = check_expr(node->left, st);
                if (rhs != TYPE_ERROR && rhs != e->dtype) {
                    char buf[200];
                    snprintf(buf, sizeof(buf),
                             "Type mismatch: '%s' %s hai, %s nahi de sakte",
                             node->sval, dtype_str(e->dtype), dtype_str(rhs));
                    sem_error(node->line, buf, NULL);
                }
                e->initialized = 1;
            }
            break;
        }

        case NODE_ARZKRO: {
            check_expr(node->left, st);
            break;
        }

        case NODE_AGAR: {
            DataType ct = check_expr(node->cond, st);
            if (ct != TYPE_BOOL && ct != TYPE_ERROR) {
                sem_warn(node->line,
                         "agar ki condition bool honi chahiye thi");
            }
            check_stmt(node->body, st);
            if (node->elseb) check_stmt(node->elseb, st);
            break;
        }

        case NODE_JABTAK: {
            DataType ct = check_expr(node->cond, st);
            if (ct != TYPE_BOOL && ct != TYPE_ERROR) {
                sem_warn(node->line,
                         "jabtak ki condition bool honi chahiye thi");
            }
            check_stmt(node->body, st);
            break;
        }

        default:
            break;
    }

    /* Process siblings handled by parent */
}

/* ─── Public entry ────────────────────────────────────────────────────────── */
int semantic_check(ASTNode *root, SymTable *st) {
    sem_errors   = 0;
    sem_warnings = 0;
    check_stmt(root, st);

    if (sem_warnings > 0)
        printf(YELLOW "[MadriZuban] %d khabardar(yan) mili.\n" RESET, sem_warnings);
    if (sem_errors > 0) {
        fprintf(stderr, RED "[MadriZuban] %d semantic ghalti(yan) — compilation rok di.\n" RESET,
                sem_errors);
        return 0;
    }
    return 1; /* clean */
}
