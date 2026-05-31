#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_LEN 256

typedef enum {
    /* Literals */
    TOK_INT_LIT,    /* 42 */
    TOK_STR_LIT,    /* "hello" */
    TOK_BOOL_LIT,   /* sach / jhoot */

    /* Types */
    TOK_INT,        /* int */
    TOK_STRING,     /* string */
    TOK_BOOL,       /* bool */

    /* MadriZuban keywords */
    TOK_ARZKRO,     /* arzkro  (print) */
    TOK_AGAR,       /* agar    (if)    */
    TOK_WARNA,      /* warna   (else)  */
    TOK_JABTAK,     /* jabtak  (while) */
    TOK_SACH,       /* sach    (true)  */
    TOK_JHOOT,      /* jhoot   (false) */

    /* Identifier */
    TOK_IDENT,

    /* Operators */
    TOK_PLUS,       /* + */
    TOK_MINUS,      /* - */
    TOK_STAR,       /* * */
    TOK_SLASH,      /* / */
    TOK_ASSIGN,     /* = */
    TOK_EQ,         /* == */
    TOK_NEQ,        /* != */
    TOK_LT,         /* <  */
    TOK_GT,         /* >  */
    TOK_LTE,        /* <= */
    TOK_GTE,        /* >= */

    /* Delimiters */
    TOK_LPAREN,     /* ( */
    TOK_RPAREN,     /* ) */
    TOK_LBRACE,     /* { */
    TOK_RBRACE,     /* } */
    TOK_SEMICOLON,  /* ; */

    TOK_EOF,
    TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char      lexeme[MAX_TOKEN_LEN];
    int       line;
} Token;

/* Lexer state */
typedef struct {
    const char *src;
    int         pos;
    int         line;
} Lexer;

void  lexer_init(Lexer *l, const char *src);
Token lexer_next(Lexer *l);
Token lexer_peek(Lexer *l);
const char *token_type_str(TokenType t);

#endif
