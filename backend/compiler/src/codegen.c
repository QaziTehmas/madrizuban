#include <stdio.h>
#include <string.h>
#include "codegen.h"

void codegen_init(CodeGenCtx *ctx, FILE *out) {
    ctx->temp_count  = 0;
    ctx->label_count = 0;
    ctx->out         = out;
}

static char *new_temp(CodeGenCtx *ctx, char *buf) {
    sprintf(buf, "t%d", ctx->temp_count++);
    return buf;
}

static char *new_label(CodeGenCtx *ctx, char *buf) {
    sprintf(buf, "L%d", ctx->label_count++);
    return buf;
}

/* Returns the name of the result (temp or literal) in result_buf */
static void gen_expr(CodeGenCtx *ctx, ASTNode *node, char *result_buf) {
    if (!node) { strcpy(result_buf, "0"); return; }

    switch (node->type) {
        case NODE_INT_LIT:
            sprintf(result_buf, "%d", node->int_val);
            return;

        case NODE_STR_LIT:
            sprintf(result_buf, "\"%s\"", node->str_val);
            return;

        case NODE_BOOL_LIT:
            sprintf(result_buf, "%s", node->bool_val ? "1" : "0");
            return;

        case NODE_IDENT:
            strcpy(result_buf, node->name);
            return;

        case NODE_BINOP: {
            char l[64], r[64], t[16];
            gen_expr(ctx, node->left,  l);
            gen_expr(ctx, node->right, r);
            new_temp(ctx, t);
            fprintf(ctx->out, "    %s = %s %s %s\n", t, l, node->op, r);
            strcpy(result_buf, t);
            return;
        }

        default:
            strcpy(result_buf, "0");
    }
}

static void gen_stmt(CodeGenCtx *ctx, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_DECL: {
            if (node->left) {
                char val[256];
                gen_expr(ctx, node->left, val);
                fprintf(ctx->out, "    %s = %s\n", node->name, val);
            } else {
                fprintf(ctx->out, "    %s = 0   ; (uninit)\n", node->name);
            }
            break;
        }

        case NODE_ASSIGN: {
            char val[256];
            gen_expr(ctx, node->left, val);
            fprintf(ctx->out, "    %s = %s\n", node->name, val);
            break;
        }

        case NODE_ARZKRO: {
            char val[256];
            gen_expr(ctx, node->left, val);
            fprintf(ctx->out, "    PRINT %s\n", val);
            break;
        }

        case NODE_AGAR: {
            char cond[64], l_else[16], l_end[16];
            gen_expr(ctx, node->cond, cond);
            new_label(ctx, l_else);
            new_label(ctx, l_end);
            fprintf(ctx->out, "    IF_FALSE %s GOTO %s\n", cond, l_else);
            gen_stmt(ctx, node->body);
            fprintf(ctx->out, "    GOTO %s\n", l_end);
            fprintf(ctx->out, "%s:\n", l_else);
            if (node->else_body) gen_stmt(ctx, node->else_body);
            fprintf(ctx->out, "%s:\n", l_end);
            break;
        }

        case NODE_JABTAK: {
            char l_start[16], l_end[16], cond[64];
            new_label(ctx, l_start);
            new_label(ctx, l_end);
            fprintf(ctx->out, "%s:\n", l_start);
            gen_expr(ctx, node->cond, cond);
            fprintf(ctx->out, "    IF_FALSE %s GOTO %s\n", cond, l_end);
            gen_stmt(ctx, node->body);
            fprintf(ctx->out, "    GOTO %s\n", l_start);
            fprintf(ctx->out, "%s:\n", l_end);
            break;
        }

        default:
            break;
    }

    gen_stmt(ctx, node->next);
}

void codegen_generate(CodeGenCtx *ctx, ASTNode *root) {
    fprintf(ctx->out, "; ===== MadriZuban Three-Address Code (TAC) =====\n");
    fprintf(ctx->out, "BEGIN_PROGRAM\n");
    if (root) gen_stmt(ctx, root->body);
    fprintf(ctx->out, "END_PROGRAM\n");
}
