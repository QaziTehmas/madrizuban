/*
 * MadriZuban Compiler v1.0
 * Air University - Compiler Construction (8th Semester)
 * Group: Affan Shafiq, Qazi Tehmas, Abdur Rahman, M.Hammad
 *
 * madrizuban.h - Main header: tokens, AST nodes, symbol table
 */

#ifndef MADRIZUBAN_H
#define MADRIZUBAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ─── Token Types ─────────────────────────────────────────────────────────── */
typedef enum {
    /* Literals */
    TOK_INT_LIT,      /* 42 */
    TOK_STRING_LIT,   /* "salam" */
    TOK_BOOL_LIT,     /* sach / jhoot */

    /* Identifiers */
    TOK_IDENT,        /* variable names */

    /* Keywords */
    TOK_INT,          /* int  */
    TOK_STRING,       /* string */
    TOK_BOOL,         /* bool */
    TOK_ARZKRO,       /* arzkro  (print) */
    TOK_AGAR,         /* agar    (if) */
    TOK_WARNA,        /* warna   (else) */
    TOK_JABTAK,       /* jabtak  (while) */
    TOK_SACH,         /* sach    (true) */
    TOK_JHOOT,        /* jhoot   (false) */

    /* Operators */
    TOK_PLUS,         /* + */
    TOK_MINUS,        /* - */
    TOK_STAR,         /* * */
    TOK_SLASH,        /* / */
    TOK_ASSIGN,       /* = */
    TOK_EQ,           /* == */
    TOK_NEQ,          /* != */
    TOK_LT,           /* < */
    TOK_GT,           /* > */
    TOK_LTE,          /* <= */
    TOK_GTE,          /* >= */

    /* Delimiters */
    TOK_LPAREN,       /* ( */
    TOK_RPAREN,       /* ) */
    TOK_LBRACE,       /* { */
    TOK_RBRACE,       /* } */
    TOK_SEMICOLON,    /* ; */

    /* Special */
    TOK_EOF,
    TOK_UNKNOWN
} TokenType;

/* ─── Token Structure ─────────────────────────────────────────────────────── */
typedef struct {
    TokenType   type;
    char        lexeme[256];
    int         line;
} Token;

/* ─── Data Types (for semantic analysis) ─────────────────────────────────── */
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_ERROR
} DataType;

/* ─── AST Node Types ──────────────────────────────────────────────────────── */
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_ARZKRO,
    NODE_AGAR,
    NODE_JABTAK,
    NODE_BLOCK,
    NODE_BINOP,
    NODE_IDENT,
    NODE_INT_LIT,
    NODE_STRING_LIT,
    NODE_BOOL_LIT
} NodeType;

/* ─── AST Node ────────────────────────────────────────────────────────────── */
typedef struct ASTNode {
    NodeType        type;
    int             line;

    /* For literals and identifiers */
    char            sval[256];   /* string value / identifier name */
    int             ival;        /* integer value */
    int             bval;        /* boolean value: 1=sach, 0=jhoot */

    /* For binary ops */
    char            op[4];

    /* For type declarations */
    DataType        dtype;

    /* Children */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;    /* condition for agar/jabtak */
    struct ASTNode *body;    /* then-branch */
    struct ASTNode *elseb;   /* warna-branch */

    /* Sibling (statement list) */
    struct ASTNode *next;
} ASTNode;

/* ─── Symbol Table Entry ──────────────────────────────────────────────────── */
typedef struct SymEntry {
    char            name[64];
    DataType        dtype;
    int             ival;
    char            sval[256];
    int             bval;
    int             initialized;
    int             line_declared;
    struct SymEntry *next;
} SymEntry;

/* ─── Symbol Table ────────────────────────────────────────────────────────── */
#define SYM_TABLE_SIZE 64
typedef struct {
    SymEntry *buckets[SYM_TABLE_SIZE];
    int       count;
} SymTable;

/* ─── Three-Address Code (TAC) ───────────────────────────────────────────── */
typedef struct TACInstr {
    char            op[16];
    char            result[64];
    char            arg1[64];
    char            arg2[64];
    struct TACInstr *next;
} TACInstr;

/* ─── Error colours (ANSI) ───────────────────────────────────────────────── */
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define GREEN   "\033[1;32m"
#define CYAN    "\033[1;36m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

/* ─── Function Prototypes ─────────────────────────────────────────────────── */

/* lexer.c */
void     lexer_init(const char *src);
Token    lexer_next(void);
Token    lexer_peek(void);
const char *token_type_str(TokenType t);

/* parser.c */
ASTNode *parser_parse(void);
void     ast_print(ASTNode *node, int indent);
void     ast_free(ASTNode *node);

/* symtable.c */
SymTable *symtable_create(void);
void      symtable_destroy(SymTable *st);
SymEntry *symtable_lookup(SymTable *st, const char *name);
int       symtable_insert(SymTable *st, const char *name, DataType dtype, int line);
void      symtable_print(SymTable *st);

/* semantic.c */
int  semantic_check(ASTNode *root, SymTable *st);
const char *dtype_str(DataType t);

/* codegen.c */
TACInstr *codegen_generate(ASTNode *root);
void      tac_print(TACInstr *list);
void      tac_free(TACInstr *list);

/* interpreter.c */
void interpreter_run(ASTNode *root, SymTable *st);

#endif /* MADRIZUBAN_H */
