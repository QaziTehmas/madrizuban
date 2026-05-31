#ifndef AST_H
#define AST_H

#include "symtable.h"

typedef enum {
    NODE_PROGRAM,
    NODE_DECL,        /* int/string/bool x = expr */
    NODE_ASSIGN,      /* x = expr */
    NODE_ARZKRO,      /* arzkro expr */
    NODE_AGAR,        /* agar (cond) { body } [warna { body }] */
    NODE_JABTAK,      /* jabtak (cond) { body } */
    NODE_BINOP,       /* expr op expr */
    NODE_INT_LIT,
    NODE_STR_LIT,
    NODE_BOOL_LIT,
    NODE_IDENT
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int      line;

    /* For literals */
    int   int_val;
    char  str_val[256];
    int   bool_val;
    char  name[64];

    /* For binop */
    char op[4];

    /* Data type for declarations */
    DataType dtype;

    /* Children */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;
    struct ASTNode *body;
    struct ASTNode *else_body;
    struct ASTNode *next;  /* next statement in block */
} ASTNode;

ASTNode *ast_new(NodeType type, int line);
void     ast_free(ASTNode *node);
void     ast_print(ASTNode *node, int indent);

#endif
