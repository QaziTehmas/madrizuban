#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symtable.h"

typedef struct {
    SymTable table;
    int      error_count;
} SemanticCtx;

void semantic_init(SemanticCtx *ctx);
int  semantic_analyse(SemanticCtx *ctx, ASTNode *root);

#endif
