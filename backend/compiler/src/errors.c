#include <stdio.h>
#include "errors.h"

void lex_error(int line, const char *msg, const char *token) {
    fprintf(stderr, RED "[%s] " RESET "Line %d: bhaiyaaaa %s → '%s'\n",
            ERR_LEXER, line, msg, token);
}

void parse_error(int line, const char *msg, const char *token) {
    fprintf(stderr, RED "[%s] " RESET "Line %d: bhaiyaaaa %s → '%s'\n",
            ERR_PARSER, line, msg, token);
}

void semantic_error(int line, const char *msg, const char *detail) {
    fprintf(stderr, RED "[%s] " RESET "Line %d: bhaiyaaaa %s → '%s'\n",
            ERR_SEMANTIC, line, msg, detail);
}

void semantic_warning(int line, const char *msg, const char *detail) {
    fprintf(stderr, YELLOW "[%s] " RESET "Line %d: bhaiyaaaa %s → '%s'\n",
            WARN_TAG, line, msg, detail);
}

void runtime_error(int line, const char *msg, const char *detail) {
    fprintf(stderr, RED "[%s] " RESET "Line %d: bhaiyaaaa %s → '%s'\n",
            ERR_RUNTIME, line, msg, detail);
}
