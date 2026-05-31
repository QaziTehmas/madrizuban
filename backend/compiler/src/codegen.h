#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

typedef struct {
    int   temp_count;
    int   label_count;
    FILE *out;
} CodeGenCtx;

void codegen_init(CodeGenCtx *ctx, FILE *out);
void codegen_generate(CodeGenCtx *ctx, ASTNode *root);

#endif
