#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "errors.h"

/* ── forward declarations ─────────────────────────────────────────────── */
static ASTNode *parse_stmt(Parser *p);
static ASTNode *parse_expr(Parser *p);
static ASTNode *parse_comparison(Parser *p);
static ASTNode *parse_additive(Parser *p);
static ASTNode *parse_multiplicative(Parser *p);
static ASTNode *parse_primary(Parser *p);
static ASTNode *parse_block(Parser *p);

/* ── helpers ──────────────────────────────────────────────────────────── */
void parser_init(Parser *p, const char *src) {
    lexer_init(&p->lexer, src);
    p->had_error   = 0;
    p->error_count = 0;
    p->current     = lexer_next(&p->lexer);
}

static Token advance_tok(Parser *p) {
    p->prev    = p->current;
    p->current = lexer_next(&p->lexer);
    return p->prev;
}

static int check(Parser *p, TokenType t) { return p->current.type == t; }

static int match(Parser *p, TokenType t) {
    if (check(p, t)) { advance_tok(p); return 1; }
    return 0;
}

static Token expect(Parser *p, TokenType t, const char *msg) {
    if (p->current.type == t) return advance_tok(p);
    parse_error(p->current.line, msg, p->current.lexeme);
    p->had_error = 1;
    p->error_count++;
    return p->current;
}

/* ── grammar ──────────────────────────────────────────────────────────── */

/* program → stmt* EOF */
ASTNode *parser_parse(Parser *p) {
    ASTNode *prog = ast_new(NODE_PROGRAM, 1);
    ASTNode **tail = &prog->body;
    while (!check(p, TOK_EOF)) {
        ASTNode *s = parse_stmt(p);
        if (s) { *tail = s; tail = &s->next; }
        if (p->had_error && p->error_count > 10) break;
    }
    return prog;
}

/* stmt → decl | assign | arzkro | agar | jabtak */
static ASTNode *parse_stmt(Parser *p) {
    int line = p->current.line;

    /* Declaration: int/string/bool ident = expr; */
    if (check(p, TOK_INT) || check(p, TOK_STRING) || check(p, TOK_BOOL)) {
        DataType dtype;
        if      (check(p, TOK_INT))    dtype = TYPE_INT;
        else if (check(p, TOK_STRING)) dtype = TYPE_STRING;
        else                           dtype = TYPE_BOOL;
        advance_tok(p);

        Token name_tok = expect(p, TOK_IDENT, "Naam chahiye tha variable ka");
        ASTNode *node = ast_new(NODE_DECL, line);
        strncpy(node->name, name_tok.lexeme, 63);
        node->dtype = dtype;

        if (match(p, TOK_ASSIGN)) {
            node->left = parse_expr(p);
        }
        expect(p, TOK_SEMICOLON, "';' chahiye tha yahan");
        return node;
    }

    /* arzkro expr; */
    if (match(p, TOK_ARZKRO)) {
        ASTNode *node = ast_new(NODE_ARZKRO, line);
        node->left = parse_expr(p);
        expect(p, TOK_SEMICOLON, "';' chahiye tha arzkro ke baad");
        return node;
    }

    /* agar (cond) { body } [warna { body }] */
    if (match(p, TOK_AGAR)) {
        ASTNode *node = ast_new(NODE_AGAR, line);
        expect(p, TOK_LPAREN, "'(' chahiye tha agar ke baad");
        node->cond = parse_expr(p);
        expect(p, TOK_RPAREN, "')' chahiye tha condition ke baad");
        node->body = parse_block(p);
        if (match(p, TOK_WARNA)) {
            node->else_body = parse_block(p);
        }
        return node;
    }

    /* jabtak (cond) { body } */
    if (match(p, TOK_JABTAK)) {
        ASTNode *node = ast_new(NODE_JABTAK, line);
        expect(p, TOK_LPAREN, "'(' chahiye tha jabtak ke baad");
        node->cond = parse_expr(p);
        expect(p, TOK_RPAREN, "')' chahiye tha condition ke baad");
        node->body = parse_block(p);
        return node;
    }

    /* Assignment: ident = expr; */
    if (check(p, TOK_IDENT)) {
        Token name_tok = advance_tok(p);
        expect(p, TOK_ASSIGN, "'=' chahiye tha assignment mein");
        ASTNode *node = ast_new(NODE_ASSIGN, line);
        strncpy(node->name, name_tok.lexeme, 63);
        node->left = parse_expr(p);
        expect(p, TOK_SEMICOLON, "';' chahiye tha yahan");
        return node;
    }

    /* Unknown token — skip and report */
    parse_error(p->current.line, "Yeh statement nahi samjha main", p->current.lexeme);
    p->had_error = 1;
    p->error_count++;
    advance_tok(p);
    return NULL;
}

/* block → '{' stmt* '}' */
static ASTNode *parse_block(Parser *p) {
    expect(p, TOK_LBRACE, "'{' chahiye tha block ke liye");
    ASTNode *first = NULL;
    ASTNode **tail = &first;
    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        ASTNode *s = parse_stmt(p);
        if (s) { *tail = s; tail = &s->next; }
    }
    expect(p, TOK_RBRACE, "'}' chahiye tha block khatam karne ke liye");
    return first;
}

/* expr → comparison */
static ASTNode *parse_expr(Parser *p) {
    return parse_comparison(p);
}

/* comparison → additive ((==|!=|<|>|<=|>=) additive)? */
static ASTNode *parse_comparison(Parser *p) {
    ASTNode *left = parse_additive(p);
    int line = p->current.line;

    char op[4] = {0};
    if      (match(p, TOK_EQ))  strcpy(op, "==");
    else if (match(p, TOK_NEQ)) strcpy(op, "!=");
    else if (match(p, TOK_LTE)) strcpy(op, "<=");
    else if (match(p, TOK_GTE)) strcpy(op, ">=");
    else if (match(p, TOK_LT))  strcpy(op, "<");
    else if (match(p, TOK_GT))  strcpy(op, ">");

    if (op[0]) {
        ASTNode *node = ast_new(NODE_BINOP, line);
        strcpy(node->op, op);
        node->left  = left;
        node->right = parse_additive(p);
        return node;
    }
    return left;
}

/* additive → multiplicative ((+|-) multiplicative)* */
static ASTNode *parse_additive(Parser *p) {
    ASTNode *left = parse_multiplicative(p);
    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        int line = p->current.line;
        char op[4];
        op[0] = p->current.lexeme[0]; op[1] = '\0';
        advance_tok(p);
        ASTNode *node = ast_new(NODE_BINOP, line);
        strcpy(node->op, op);
        node->left  = left;
        node->right = parse_multiplicative(p);
        left = node;
    }
    return left;
}

/* multiplicative → primary ((*|/) primary)* */
static ASTNode *parse_multiplicative(Parser *p) {
    ASTNode *left = parse_primary(p);
    while (check(p, TOK_STAR) || check(p, TOK_SLASH)) {
        int line = p->current.line;
        char op[4];
        op[0] = p->current.lexeme[0]; op[1] = '\0';
        advance_tok(p);
        ASTNode *node = ast_new(NODE_BINOP, line);
        strcpy(node->op, op);
        node->left  = left;
        node->right = parse_primary(p);
        left = node;
    }
    return left;
}

/* primary → INT_LIT | STR_LIT | sach | jhoot | IDENT | '(' expr ')' */
static ASTNode *parse_primary(Parser *p) {
    int line = p->current.line;

    if (check(p, TOK_INT_LIT)) {
        ASTNode *n = ast_new(NODE_INT_LIT, line);
        n->int_val = atoi(p->current.lexeme);
        advance_tok(p);
        return n;
    }
    if (check(p, TOK_STR_LIT)) {
        ASTNode *n = ast_new(NODE_STR_LIT, line);
        strncpy(n->str_val, p->current.lexeme, 255);
        advance_tok(p);
        return n;
    }
    if (check(p, TOK_SACH)) {
        ASTNode *n = ast_new(NODE_BOOL_LIT, line);
        n->bool_val = 1;
        advance_tok(p);
        return n;
    }
    if (check(p, TOK_JHOOT)) {
        ASTNode *n = ast_new(NODE_BOOL_LIT, line);
        n->bool_val = 0;
        advance_tok(p);
        return n;
    }
    if (check(p, TOK_IDENT)) {
        ASTNode *n = ast_new(NODE_IDENT, line);
        strncpy(n->name, p->current.lexeme, 63);
        advance_tok(p);
        return n;
    }
    if (match(p, TOK_LPAREN)) {
        ASTNode *e = parse_expr(p);
        expect(p, TOK_RPAREN, "')' chahiye tha expression ke baad");
        return e;
    }

    parse_error(line, "Expression expect tha yahan", p->current.lexeme);
    p->had_error = 1;
    p->error_count++;
    advance_tok(p);
    return ast_new(NODE_INT_LIT, line); /* dummy */
}
