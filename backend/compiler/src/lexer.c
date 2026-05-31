#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

void lexer_init(Lexer *l, const char *src) {
    l->src  = src;
    l->pos  = 0;
    l->line = 1;
}

static char peek_char(Lexer *l) { return l->src[l->pos]; }
static char advance(Lexer *l)   { char c = l->src[l->pos]; if (c) l->pos++; if (c == '\n') l->line++; return c; }

static void skip_whitespace_comments(Lexer *l) {
    while (1) {
        char c = peek_char(l);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance(l);
        } else if (c == '/' && l->src[l->pos+1] == '/') {
            /* single-line comment */
            while (peek_char(l) && peek_char(l) != '\n') advance(l);
        } else if (c == '/' && l->src[l->pos+1] == '*') {
            /* block comment */
            advance(l); advance(l);
            while (peek_char(l)) {
                if (peek_char(l) == '*' && l->src[l->pos+1] == '/') {
                    advance(l); advance(l); break;
                }
                advance(l);
            }
        } else {
            break;
        }
    }
}

static Token make_tok(TokenType t, const char *lex, int line) {
    Token tok;
    tok.type = t;
    tok.line = line;
    strncpy(tok.lexeme, lex, MAX_TOKEN_LEN - 1);
    tok.lexeme[MAX_TOKEN_LEN - 1] = '\0';
    return tok;
}

Token lexer_next(Lexer *l) {
    skip_whitespace_comments(l);
    int line = l->line;
    char c = peek_char(l);

    if (!c) return make_tok(TOK_EOF, "EOF", line);

    /* String literal */
    if (c == '"') {
        advance(l);
        char buf[MAX_TOKEN_LEN]; int bi = 0;
        while (peek_char(l) && peek_char(l) != '"') {
            if (bi < MAX_TOKEN_LEN - 1) buf[bi++] = advance(l);
            else advance(l);
        }
        if (peek_char(l) == '"') advance(l);
        buf[bi] = '\0';
        return make_tok(TOK_STR_LIT, buf, line);
    }

    /* Number */
    if (isdigit(c)) {
        char buf[MAX_TOKEN_LEN]; int bi = 0;
        while (isdigit(peek_char(l)) && bi < MAX_TOKEN_LEN - 1)
            buf[bi++] = advance(l);
        buf[bi] = '\0';
        return make_tok(TOK_INT_LIT, buf, line);
    }

    /* Identifier / keyword */
    if (isalpha(c) || c == '_') {
        char buf[MAX_TOKEN_LEN]; int bi = 0;
        while ((isalnum(peek_char(l)) || peek_char(l) == '_') && bi < MAX_TOKEN_LEN - 1)
            buf[bi++] = advance(l);
        buf[bi] = '\0';

        if (strcmp(buf, "int")    == 0) return make_tok(TOK_INT,    buf, line);
        if (strcmp(buf, "string") == 0) return make_tok(TOK_STRING, buf, line);
        if (strcmp(buf, "bool")   == 0) return make_tok(TOK_BOOL,   buf, line);
        if (strcmp(buf, "arzkro") == 0) return make_tok(TOK_ARZKRO, buf, line);
        if (strcmp(buf, "agar")   == 0) return make_tok(TOK_AGAR,   buf, line);
        if (strcmp(buf, "warna")  == 0) return make_tok(TOK_WARNA,  buf, line);
        if (strcmp(buf, "jabtak") == 0) return make_tok(TOK_JABTAK, buf, line);
        if (strcmp(buf, "sach")   == 0) return make_tok(TOK_SACH,   buf, line);
        if (strcmp(buf, "jhoot")  == 0) return make_tok(TOK_JHOOT,  buf, line);
        return make_tok(TOK_IDENT, buf, line);
    }

    /* Two-char operators */
    advance(l);
    char nc = peek_char(l);

    if (c == '=' && nc == '=') { advance(l); return make_tok(TOK_EQ,  "==", line); }
    if (c == '!' && nc == '=') { advance(l); return make_tok(TOK_NEQ, "!=", line); }
    if (c == '<' && nc == '=') { advance(l); return make_tok(TOK_LTE, "<=", line); }
    if (c == '>' && nc == '=') { advance(l); return make_tok(TOK_GTE, ">=", line); }

    /* Single-char */
    char buf[2] = {c, '\0'};
    switch (c) {
        case '+': return make_tok(TOK_PLUS,      buf, line);
        case '-': return make_tok(TOK_MINUS,     buf, line);
        case '*': return make_tok(TOK_STAR,      buf, line);
        case '/': return make_tok(TOK_SLASH,     buf, line);
        case '=': return make_tok(TOK_ASSIGN,    buf, line);
        case '<': return make_tok(TOK_LT,        buf, line);
        case '>': return make_tok(TOK_GT,        buf, line);
        case '(': return make_tok(TOK_LPAREN,    buf, line);
        case ')': return make_tok(TOK_RPAREN,    buf, line);
        case '{': return make_tok(TOK_LBRACE,    buf, line);
        case '}': return make_tok(TOK_RBRACE,    buf, line);
        case ';': return make_tok(TOK_SEMICOLON, buf, line);
    }

    char ub[2] = {c, '\0'};
    return make_tok(TOK_UNKNOWN, ub, line);
}

Token lexer_peek(Lexer *l) {
    Lexer save = *l;
    Token t = lexer_next(l);
    *l = save;
    return t;
}

const char *token_type_str(TokenType t) {
    switch (t) {
        case TOK_INT_LIT:   return "INT_LITERAL";
        case TOK_STR_LIT:   return "STR_LITERAL";
        case TOK_BOOL_LIT:  return "BOOL_LITERAL";
        case TOK_INT:       return "int";
        case TOK_STRING:    return "string";
        case TOK_BOOL:      return "bool";
        case TOK_ARZKRO:    return "arzkro";
        case TOK_AGAR:      return "agar";
        case TOK_WARNA:     return "warna";
        case TOK_JABTAK:    return "jabtak";
        case TOK_SACH:      return "sach";
        case TOK_JHOOT:     return "jhoot";
        case TOK_IDENT:     return "IDENTIFIER";
        case TOK_PLUS:      return "+";
        case TOK_MINUS:     return "-";
        case TOK_STAR:      return "*";
        case TOK_SLASH:     return "/";
        case TOK_ASSIGN:    return "=";
        case TOK_EQ:        return "==";
        case TOK_NEQ:       return "!=";
        case TOK_LT:        return "<";
        case TOK_GT:        return ">";
        case TOK_LTE:       return "<=";
        case TOK_GTE:       return ">=";
        case TOK_LPAREN:    return "(";
        case TOK_RPAREN:    return ")";
        case TOK_LBRACE:    return "{";
        case TOK_RBRACE:    return "}";
        case TOK_SEMICOLON: return ";";
        case TOK_EOF:       return "EOF";
        default:            return "UNKNOWN";
    }
}
