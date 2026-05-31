#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "symtable.h"

typedef struct {
    SymTable table;
    int      had_runtime_error;
} Interpreter;

void interp_init(Interpreter *interp);
void interp_run(Interpreter *interp, ASTNode *root);

#endif
