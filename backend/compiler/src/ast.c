#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *ast_new(NodeType type, int line) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;
    n->line = line;
    return n;
}

void ast_free(ASTNode *node) {
    if (!node) return;
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->cond);
    ast_free(node->body);
    ast_free(node->else_body);
    ast_free(node->next);
    free(node);
}

static const char *node_type_str(NodeType t) {
    switch (t) {
        case NODE_PROGRAM:  return "PROGRAM";
        case NODE_DECL:     return "DECL";
        case NODE_ASSIGN:   return "ASSIGN";
        case NODE_ARZKRO:   return "ARZKRO";
        case NODE_AGAR:     return "AGAR";
        case NODE_JABTAK:   return "JABTAK";
        case NODE_BINOP:    return "BINOP";
        case NODE_INT_LIT:  return "INT_LIT";
        case NODE_STR_LIT:  return "STR_LIT";
        case NODE_BOOL_LIT: return "BOOL_LIT";
        case NODE_IDENT:    return "IDENT";
        default:            return "?";
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    printf("[%s]", node_type_str(node->type));
    if (node->type == NODE_INT_LIT)  printf(" %d", node->int_val);
    if (node->type == NODE_STR_LIT)  printf(" \"%s\"", node->str_val);
    if (node->type == NODE_BOOL_LIT) printf(" %s", node->bool_val ? "sach" : "jhoot");
    if (node->type == NODE_IDENT)    printf(" %s", node->name);
    if (node->type == NODE_BINOP)    printf(" '%s'", node->op);
    if (node->type == NODE_DECL)     printf(" %s", node->name);
    if (node->type == NODE_ASSIGN)   printf(" %s", node->name);
    printf("\n");

    ast_print(node->cond,      indent + 1);
    ast_print(node->left,      indent + 1);
    ast_print(node->right,     indent + 1);
    ast_print(node->body,      indent + 1);
    ast_print(node->else_body, indent + 1);
    ast_print(node->next,      indent);
}
