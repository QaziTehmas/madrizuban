#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer  lexer;
    Token  current;
    Token  prev;
    int    had_error;
    int    error_count;
} Parser;

void     parser_init(Parser *p, const char *src);
ASTNode *parser_parse(Parser *p);

#endif
