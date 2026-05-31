#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "errors.h"

void interp_init(Interpreter *interp) {
    sym_init(&interp->table);
    interp->had_runtime_error = 0;
}

typedef struct {
    DataType type;
    int      int_val;
    char     str_val[256];
    int      bool_val;
} Val;

static Val make_int(int v)          { Val r; r.type = TYPE_INT;    r.int_val  = v; r.str_val[0]='\0'; r.bool_val=0; return r; }
static Val make_str(const char *s)  { Val r; r.type = TYPE_STRING; r.int_val  = 0; strncpy(r.str_val,s,255); r.bool_val=0; return r; }
static Val make_bool(int v)         { Val r; r.type = TYPE_BOOL;   r.int_val  = 0; r.str_val[0]='\0'; r.bool_val=v; return r; }

static Val eval_expr(Interpreter *interp, ASTNode *node);

static Val eval_expr(Interpreter *interp, ASTNode *node) {
    if (!node) return make_int(0);

    switch (node->type) {
        case NODE_INT_LIT:  return make_int(node->int_val);
        case NODE_STR_LIT:  return make_str(node->str_val);
        case NODE_BOOL_LIT: return make_bool(node->bool_val);

        case NODE_IDENT: {
            Symbol *s = sym_lookup(&interp->table, node->name);
            if (!s) {
                runtime_error(node->line,
                    "Yeh variable runtime pe nahi mila", node->name);
                interp->had_runtime_error = 1;
                return make_int(0);
            }
            if (s->type == TYPE_STRING) return make_str(s->value.str_val);
            if (s->type == TYPE_BOOL)   return make_bool(s->value.bool_val);
            return make_int(s->value.int_val);
        }

        case NODE_BINOP: {
            Val l = eval_expr(interp, node->left);
            Val r = eval_expr(interp, node->right);

            /* Comparisons */
            if (strcmp(node->op, "==") == 0) {
                if (l.type == TYPE_STRING && r.type == TYPE_STRING)
                    return make_bool(strcmp(l.str_val, r.str_val) == 0);
                return make_bool(l.int_val == r.int_val);
            }
            if (strcmp(node->op, "!=") == 0) return make_bool(l.int_val != r.int_val);
            if (strcmp(node->op, "<")  == 0) return make_bool(l.int_val <  r.int_val);
            if (strcmp(node->op, ">")  == 0) return make_bool(l.int_val >  r.int_val);
            if (strcmp(node->op, "<=") == 0) return make_bool(l.int_val <= r.int_val);
            if (strcmp(node->op, ">=") == 0) return make_bool(l.int_val >= r.int_val);

            /* Arithmetic */
            if (strcmp(node->op, "+") == 0) return make_int(l.int_val + r.int_val);
            if (strcmp(node->op, "-") == 0) return make_int(l.int_val - r.int_val);
            if (strcmp(node->op, "*") == 0) return make_int(l.int_val * r.int_val);
            if (strcmp(node->op, "/") == 0) {
                if (r.int_val == 0) {
                    runtime_error(node->line,
                        "Zero se taqseem nahi hoti bhai", "/");
                    interp->had_runtime_error = 1;
                    return make_int(0);
                }
                return make_int(l.int_val / r.int_val);
            }
            return make_int(0);
        }

        default:
            return make_int(0);
    }
}

static void exec_stmt(Interpreter *interp, ASTNode *node) {
    if (!node || interp->had_runtime_error) return;

    switch (node->type) {
        case NODE_DECL: {
            sym_declare(&interp->table, node->name, node->dtype);
            if (node->left) {
                Val v = eval_expr(interp, node->left);
                if (node->dtype == TYPE_STRING) sym_set_str(&interp->table, node->name, v.str_val);
                else if (node->dtype == TYPE_BOOL) sym_set_bool(&interp->table, node->name, v.bool_val);
                else sym_set_int(&interp->table, node->name, v.int_val);
            }
            break;
        }

        case NODE_ASSIGN: {
            Val v = eval_expr(interp, node->left);
            Symbol *s = sym_lookup(&interp->table, node->name);
            if (!s) {
                runtime_error(node->line,
                    "Yeh variable kabhi bataya nahi tha", node->name);
                interp->had_runtime_error = 1;
                break;
            }
            if (s->type == TYPE_STRING) sym_set_str(&interp->table, node->name, v.str_val);
            else if (s->type == TYPE_BOOL) sym_set_bool(&interp->table, node->name, v.bool_val);
            else sym_set_int(&interp->table, node->name, v.int_val);
            break;
        }

        case NODE_ARZKRO: {
            Val v = eval_expr(interp, node->left);
            if (v.type == TYPE_STRING) printf("%s\n", v.str_val);
            else if (v.type == TYPE_BOOL) printf("%s\n", v.bool_val ? "sach" : "jhoot");
            else printf("%d\n", v.int_val);
            break;
        }

        case NODE_AGAR: {
            Val cond = eval_expr(interp, node->cond);
            int truth = (cond.type == TYPE_BOOL) ? cond.bool_val : cond.int_val;
            if (truth) exec_stmt(interp, node->body);
            else if (node->else_body) exec_stmt(interp, node->else_body);
            break;
        }

        case NODE_JABTAK: {
            int max_iter = 100000;
            while (max_iter-- > 0) {
                Val cond = eval_expr(interp, node->cond);
                int truth = (cond.type == TYPE_BOOL) ? cond.bool_val : cond.int_val;
                if (!truth) break;
                exec_stmt(interp, node->body);
                if (interp->had_runtime_error) break;
            }
            if (max_iter <= 0) {
                runtime_error(node->line,
                    "Yeh loop khatam nahi ho raha tha bhai — roka", "jabtak");
                interp->had_runtime_error = 1;
            }
            break;
        }

        default:
            break;
    }

    exec_stmt(interp, node->next);
}

void interp_run(Interpreter *interp, ASTNode *root) {
    if (!root) return;
    exec_stmt(interp, root->body);
}
