/*
 * MadriZuban Compiler v1.0
 * codegen.c - Intermediate Code Generation (Three-Address Code)
 *
 * Produces TAC instructions of the form:
 *   result = arg1 op arg2   (BINOP)
 *   result = arg1            (COPY / PARAM)
 *   PRINT arg1               (arzkro)
 *   IF arg1 GOTO label       (conditional jump)
 *   GOTO label               (unconditional jump)
 *   LABEL label:             (label definition)
 */

#include "madrizuban.h"

/* ─── Code generation state ───────────────────────────────────────────────── */
static TACInstr *tac_head = NULL;
static TACInstr *tac_tail = NULL;
static int       temp_ctr = 0;
static int       label_ctr = 0;

/* ─── Helpers ─────────────────────────────────────────────────────────────── */
static char *new_temp(void) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "t%d", temp_ctr++);
    return buf;
}

static char *new_label(void) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "L%d", label_ctr++);
    return buf;
}

static void emit(const char *op, const char *result,
                 const char *arg1, const char *arg2) {
    TACInstr *instr = (TACInstr *)calloc(1, sizeof(TACInstr));
    if (op)     strncpy(instr->op,     op,     15);
    if (result) strncpy(instr->result, result, 63);
    if (arg1)   strncpy(instr->arg1,   arg1,   63);
    if (arg2)   strncpy(instr->arg2,   arg2,   63);

    if (!tac_head) {
        tac_head = tac_tail = instr;
    } else {
        tac_tail->next = instr;
        tac_tail = instr;
    }
}

/* ─── Forward ─────────────────────────────────────────────────────────────── */
static char *gen_expr(ASTNode *node);
static void  gen_stmt(ASTNode *node);

/* ─── Generate expression, returns temp/literal name holding value ────────── */
static char *gen_expr(ASTNode *node) {
    static char literal_buf[128];

    if (!node) return "_";

    switch (node->type) {

        case NODE_INT_LIT:
            snprintf(literal_buf, sizeof(literal_buf), "%d", node->ival);
            return literal_buf;

        case NODE_STRING_LIT:
            snprintf(literal_buf, sizeof(literal_buf), "\"%s\"", node->sval);
            return literal_buf;

        case NODE_BOOL_LIT:
            snprintf(literal_buf, sizeof(literal_buf), "%s",
                     node->bval ? "sach" : "jhoot");
            return literal_buf;

        case NODE_IDENT:
            return node->sval;

        case NODE_BINOP: {
            char *l   = gen_expr(node->left);
            char lbuf[64]; strncpy(lbuf, l, 63); /* snapshot before recursive call */
            char *r   = gen_expr(node->right);
            char *tmp = new_temp();

            /* Handle comparison ops */
            if (strcmp(node->op,"==")==0 || strcmp(node->op,"!=")==0 ||
                strcmp(node->op,"<") ==0 || strcmp(node->op,">") ==0 ||
                strcmp(node->op,"<=")==0 || strcmp(node->op,">=")==0) {
                emit("CMP", tmp, lbuf, r);
                /* Store operator so print phase can show it */
                char cmp_op_buf[96];
                snprintf(cmp_op_buf, sizeof(cmp_op_buf), "%s%s%s", lbuf, node->op, r);
                /* overwrite result with annotated form */
                snprintf(tac_tail->result, 63, "%s", tmp);
                snprintf(tac_tail->arg1, 63, "%s", lbuf);
                snprintf(tac_tail->arg2, 63, "%s %s", node->op, r);
            } else {
                emit(node->op, tmp, lbuf, r);
            }
            return tmp;
        }

        default:
            return "_";
    }
}

/* ─── Generate statement ──────────────────────────────────────────────────── */
static void gen_stmt(ASTNode *node) {
    if (!node) return;

    switch (node->type) {

        case NODE_PROGRAM:
        case NODE_BLOCK: {
            ASTNode *s = node->body;
            while (s) { gen_stmt(s); s = s->next; }
            break;
        }

        case NODE_VAR_DECL: {
            char *rhs = gen_expr(node->left);
            emit("DECL", node->sval, rhs, dtype_str(node->dtype));
            break;
        }

        case NODE_ASSIGN: {
            char *rhs = gen_expr(node->left);
            char rbuf[64]; strncpy(rbuf, rhs, 63);
            emit("=", node->sval, rbuf, NULL);
            break;
        }

        case NODE_ARZKRO: {
            char *val = gen_expr(node->left);
            char vbuf[64]; strncpy(vbuf, val, 63);
            emit("PRINT", vbuf, NULL, NULL);
            break;
        }

        case NODE_AGAR: {
            char *cond       = gen_expr(node->cond);
            char cond_buf[64]; strncpy(cond_buf, cond, 63);
            char *label_else = new_label();
            char *label_end  = new_label();

            emit("IFFALSE", cond_buf, "GOTO", label_else);
            gen_stmt(node->body);
            emit("GOTO", label_end, NULL, NULL);
            emit("LABEL", label_else, NULL, NULL);
            if (node->elseb) gen_stmt(node->elseb);
            emit("LABEL", label_end, NULL, NULL);
            break;
        }

        case NODE_JABTAK: {
            char *label_start = new_label();
            char *label_end   = new_label();

            emit("LABEL", label_start, NULL, NULL);
            char *cond = gen_expr(node->cond);
            char cond_buf[64]; strncpy(cond_buf, cond, 63);
            emit("IFFALSE", cond_buf, "GOTO", label_end);
            gen_stmt(node->body);
            emit("GOTO", label_start, NULL, NULL);
            emit("LABEL", label_end, NULL, NULL);
            break;
        }

        default:
            break;
    }
}

/* ─── Public: generate TAC from AST ──────────────────────────────────────── */
TACInstr *codegen_generate(ASTNode *root) {
    tac_head = tac_tail = NULL;
    temp_ctr = label_ctr = 0;
    gen_stmt(root);
    return tac_head;
}

/* ─── Print TAC ───────────────────────────────────────────────────────────── */
void tac_print(TACInstr *list) {
    printf(CYAN "\n=== Three-Address Code (TAC) ===\n" RESET);
    int i = 1;
    for (TACInstr *p = list; p; p = p->next, i++) {
        if (strcmp(p->op, "LABEL") == 0) {
            printf(GREEN "  %s:\n" RESET, p->result);
        } else if (strcmp(p->op, "PRINT") == 0) {
            printf("  %3d │  PRINT %s\n", i, p->result);
        } else if (strcmp(p->op, "DECL") == 0) {
            printf("  %3d │  DECL %s (%s) = %s\n", i, p->result, p->arg2, p->arg1);
        } else if (strcmp(p->op, "GOTO") == 0) {
            printf("  %3d │  GOTO %s\n", i, p->result);
        } else if (strcmp(p->op, "IFFALSE") == 0) {
            printf("  %3d │  IFFALSE %s GOTO %s\n", i, p->result, p->arg2);
        } else if (strcmp(p->op, "=") == 0) {
            printf("  %3d │  %s = %s\n", i, p->result, p->arg1);
        } else if (strcmp(p->op, "CMP") == 0) {
            printf("  %3d │  %s = %s %s\n", i, p->result, p->arg1, p->arg2);
        } else {
            printf("  %3d │  %s = %s %s %s\n", i,
                   p->result, p->arg1, p->op, p->arg2);
        }
    }
    printf("\n");
}

/* ─── Free TAC list ───────────────────────────────────────────────────────── */
void tac_free(TACInstr *list) {
    while (list) {
        TACInstr *tmp = list->next;
        free(list);
        list = tmp;
    }
}
