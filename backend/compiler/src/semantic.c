#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "errors.h"

void semantic_init(SemanticCtx *ctx) {
    sym_init(&ctx->table);
    ctx->error_count = 0;
}

static DataType check_expr(SemanticCtx *ctx, ASTNode *node);

static DataType check_expr(SemanticCtx *ctx, ASTNode *node) {
    if (!node) return TYPE_INT;

    switch (node->type) {
        case NODE_INT_LIT:
            return TYPE_INT;

        case NODE_STR_LIT:
            return TYPE_STRING;

        case NODE_BOOL_LIT:
            return TYPE_BOOL;

        case NODE_IDENT: {
            Symbol *s = sym_lookup(&ctx->table, node->name);
            if (!s) {
                semantic_error(node->line,
                    "Yeh variable kabhi bataya nahi tha", node->name);
                ctx->error_count++;
                return TYPE_INT;
            }
            return s->type;
        }

        case NODE_BINOP: {
            DataType lt = check_expr(ctx, node->left);
            DataType rt = check_expr(ctx, node->right);

            /* Comparison operators return bool */
            if (strcmp(node->op, "==") == 0 || strcmp(node->op, "!=") == 0 ||
                strcmp(node->op, "<")  == 0 || strcmp(node->op, ">")  == 0 ||
                strcmp(node->op, "<=") == 0 || strcmp(node->op, ">=") == 0) {
                return TYPE_BOOL;
            }

            /* Arithmetic only valid on int */
            if (lt != TYPE_INT || rt != TYPE_INT) {
                semantic_error(node->line,
                    "Arithmetic sirf integers pe hoti hai", node->op);
                ctx->error_count++;
            }

            /* Warn on literal division by zero */
            if (strcmp(node->op, "/") == 0 &&
                node->right && node->right->type == NODE_INT_LIT &&
                node->right->int_val == 0) {
                semantic_warning(node->line,
                    "Zero se taqseem? Serious ho tum bhai", "/");
            }
            return TYPE_INT;
        }

        default:
            return TYPE_INT;
    }
}

static void check_stmt(SemanticCtx *ctx, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_DECL: {
            int r = sym_declare(&ctx->table, node->name, node->dtype);
            if (r == -1) {
                semantic_error(node->line,
                    "Yeh variable pehle se mojood hai", node->name);
                ctx->error_count++;
            }
            if (node->left) {
                DataType et = check_expr(ctx, node->left);
                /* Type compatibility check */
                if (node->dtype == TYPE_INT && et == TYPE_STRING) {
                    semantic_error(node->line,
                        "String ko int mein nahi dal sakte", node->name);
                    ctx->error_count++;
                }
                if (node->dtype == TYPE_STRING && et == TYPE_INT) {
                    semantic_error(node->line,
                        "Int ko string mein nahi dal sakte", node->name);
                    ctx->error_count++;
                }
            }
            break;
        }

        case NODE_ASSIGN: {
            Symbol *s = sym_lookup(&ctx->table, node->name);
            if (!s) {
                semantic_error(node->line,
                    "Yeh variable kabhi bataya nahi tha", node->name);
                ctx->error_count++;
            } else {
                DataType et = check_expr(ctx, node->left);
                if (s->type == TYPE_INT && et == TYPE_STRING) {
                    semantic_error(node->line,
                        "String ko int variable mein nahi rakh sakte", node->name);
                    ctx->error_count++;
                }
            }
            break;
        }

        case NODE_ARZKRO:
            check_expr(ctx, node->left);
            break;

        case NODE_AGAR:
            check_expr(ctx, node->cond);
            check_stmt(ctx, node->body);
            if (node->else_body) check_stmt(ctx, node->else_body);
            break;

        case NODE_JABTAK:
            check_expr(ctx, node->cond);
            check_stmt(ctx, node->body);
            break;

        default:
            break;
    }

    check_stmt(ctx, node->next);
}

int semantic_analyse(SemanticCtx *ctx, ASTNode *root) {
    if (!root) return 0;
    check_stmt(ctx, root->body);
    return ctx->error_count;
}
