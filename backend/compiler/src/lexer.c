/*
 * MadriZuban Compiler v1.0
 * lexer.c - Lexical Analyser
 *
 * Reads source characters and emits Tokens.
 * Handles: keywords, identifiers, int/string/bool literals,
 *          operators (including two-char), delimiters, comments.
 */

#include "madrizuban.h"

/* ─── Lexer state ─────────────────────────────────────────────────────────── */
static const char *src_buf   = NULL;
static int         src_pos   = 0;
static int         src_len   = 0;
static int         cur_line  = 1;
static Token       peeked;
static int         has_peeked = 0;

/* ─── Helpers ─────────────────────────────────────────────────────────────── */
static char peek_char(void) {
    if (src_pos >= src_len) return '\0';
    return src_buf[src_pos];
}

static char advance_char(void) {
    char c = src_buf[src_pos++];
    if (c == '\n') cur_line++;
    return c;
}

static void skip_whitespace_and_comments(void) {
    for (;;) {
        /* Skip whitespace */
        while (src_pos < src_len && isspace((unsigned char)peek_char()))
            advance_char();

        /* Single-line comment // */
        if (src_pos + 1 < src_len &&
            src_buf[src_pos] == '/' && src_buf[src_pos+1] == '/') {
            while (src_pos < src_len && peek_char() != '\n')
                advance_char();
            continue;
        }

        /* Block comment: scan until star-slash */
        if (src_pos + 1 < src_len &&
            src_buf[src_pos] == '/' && src_buf[src_pos+1] == '*') {
            src_pos += 2;
            while (src_pos + 1 < src_len &&
                   !(src_buf[src_pos] == '*' && src_buf[src_pos+1] == '/')) {
                if (src_buf[src_pos] == '\n') cur_line++;
                src_pos++;
            }
            if (src_pos + 1 < src_len) src_pos += 2; /* consume closing */
            continue;
        }

        break;
    }
}

/* Map identifier string to keyword token type (or TOK_IDENT) */
static TokenType classify_keyword(const char *s) {
    if (strcmp(s, "int")     == 0) return TOK_INT;
    if (strcmp(s, "string")  == 0) return TOK_STRING;
    if (strcmp(s, "bool")    == 0) return TOK_BOOL;
    if (strcmp(s, "arzkro")  == 0) return TOK_ARZKRO;
    if (strcmp(s, "agar")    == 0) return TOK_AGAR;
    if (strcmp(s, "warna")   == 0) return TOK_WARNA;
    if (strcmp(s, "jabtak")  == 0) return TOK_JABTAK;
    if (strcmp(s, "sach")    == 0) return TOK_SACH;
    if (strcmp(s, "jhoot")   == 0) return TOK_JHOOT;
    return TOK_IDENT;
}

/* ─── Public: initialise lexer ────────────────────────────────────────────── */
void lexer_init(const char *src) {
    src_buf    = src;
    src_pos    = 0;
    src_len    = (int)strlen(src);
    cur_line   = 1;
    has_peeked = 0;
}

/* ─── Core: produce next token ────────────────────────────────────────────── */
Token lexer_next(void) {
    if (has_peeked) {
        has_peeked = 0;
        return peeked;
    }

    Token tok;
    memset(&tok, 0, sizeof(tok));

    skip_whitespace_and_comments();
    tok.line = cur_line;

    if (src_pos >= src_len) {
        tok.type = TOK_EOF;
        strcpy(tok.lexeme, "EOF");
        return tok;
    }

    char c = peek_char();

    /* ── Integer literal ── */
    if (isdigit((unsigned char)c)) {
        int i = 0;
        while (src_pos < src_len && isdigit((unsigned char)peek_char()))
            tok.lexeme[i++] = advance_char();
        tok.lexeme[i] = '\0';
        tok.type = TOK_INT_LIT;
        return tok;
    }

    /* ── Identifier / keyword ── */
    if (isalpha((unsigned char)c) || c == '_') {
        int i = 0;
        while (src_pos < src_len &&
               (isalnum((unsigned char)peek_char()) || peek_char() == '_'))
            tok.lexeme[i++] = advance_char();
        tok.lexeme[i] = '\0';
        tok.type = classify_keyword(tok.lexeme);

        /* sach / jhoot are bool literals */
        if (tok.type == TOK_SACH || tok.type == TOK_JHOOT)
            tok.type = (tok.type == TOK_SACH) ? TOK_SACH : TOK_JHOOT;

        return tok;
    }

    /* ── String literal ── */
    if (c == '"') {
        advance_char(); /* consume opening quote */
        int i = 0;
        while (src_pos < src_len && peek_char() != '"') {
            if (peek_char() == '\\') {
                advance_char();
                char esc = advance_char();
                switch (esc) {
                    case 'n':  tok.lexeme[i++] = '\n'; break;
                    case 't':  tok.lexeme[i++] = '\t'; break;
                    default:   tok.lexeme[i++] = esc;  break;
                }
            } else {
                tok.lexeme[i++] = advance_char();
            }
        }
        if (src_pos < src_len) advance_char(); /* consume closing quote */
        tok.lexeme[i] = '\0';
        tok.type = TOK_STRING_LIT;
        return tok;
    }

    /* ── Two-character operators ── */
    advance_char(); /* consume first char */

    if (c == '=' && peek_char() == '=') { advance_char(); tok.type = TOK_EQ;  strcpy(tok.lexeme, "=="); return tok; }
    if (c == '!' && peek_char() == '=') { advance_char(); tok.type = TOK_NEQ; strcpy(tok.lexeme, "!="); return tok; }
    if (c == '<' && peek_char() == '=') { advance_char(); tok.type = TOK_LTE; strcpy(tok.lexeme, "<="); return tok; }
    if (c == '>' && peek_char() == '=') { advance_char(); tok.type = TOK_GTE; strcpy(tok.lexeme, ">="); return tok; }

    /* ── Single-character operators / delimiters ── */
    tok.lexeme[0] = c;
    tok.lexeme[1] = '\0';
    switch (c) {
        case '+': tok.type = TOK_PLUS;      break;
        case '-': tok.type = TOK_MINUS;     break;
        case '*': tok.type = TOK_STAR;      break;
        case '/': tok.type = TOK_SLASH;     break;
        case '=': tok.type = TOK_ASSIGN;    break;
        case '<': tok.type = TOK_LT;        break;
        case '>': tok.type = TOK_GT;        break;
        case '(': tok.type = TOK_LPAREN;    break;
        case ')': tok.type = TOK_RPAREN;    break;
        case '{': tok.type = TOK_LBRACE;    break;
        case '}': tok.type = TOK_RBRACE;    break;
        case ';': tok.type = TOK_SEMICOLON; break;
        default:
            tok.type = TOK_UNKNOWN;
            fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
                    " bhaiyaaaa Line %d: Yeh haroof nahi pehchana → '%c'\n",
                    cur_line, c);
            break;
    }
    return tok;
}

/* ─── Peek one token ahead ────────────────────────────────────────────────── */
Token lexer_peek(void) {
    if (!has_peeked) {
        peeked     = lexer_next();
        has_peeked = 1;
    }
    return peeked;
}

/* ─── Debug: token type as string ─────────────────────────────────────────── */
const char *token_type_str(TokenType t) {
    switch (t) {
        case TOK_INT_LIT:    return "INT_LIT";
        case TOK_STRING_LIT: return "STRING_LIT";
        case TOK_BOOL_LIT:   return "BOOL_LIT";
        case TOK_IDENT:      return "IDENT";
        case TOK_INT:        return "int";
        case TOK_STRING:     return "string";
        case TOK_BOOL:       return "bool";
        case TOK_ARZKRO:     return "arzkro";
        case TOK_AGAR:       return "agar";
        case TOK_WARNA:      return "warna";
        case TOK_JABTAK:     return "jabtak";
        case TOK_SACH:       return "sach";
        case TOK_JHOOT:      return "jhoot";
        case TOK_PLUS:       return "+";
        case TOK_MINUS:      return "-";
        case TOK_STAR:       return "*";
        case TOK_SLASH:      return "/";
        case TOK_ASSIGN:     return "=";
        case TOK_EQ:         return "==";
        case TOK_NEQ:        return "!=";
        case TOK_LT:         return "<";
        case TOK_GT:         return ">";
        case TOK_LTE:        return "<=";
        case TOK_GTE:        return ">=";
        case TOK_LPAREN:     return "(";
        case TOK_RPAREN:     return ")";
        case TOK_LBRACE:     return "{";
        case TOK_RBRACE:     return "}";
        case TOK_SEMICOLON:  return ";";
        case TOK_EOF:        return "EOF";
        default:             return "UNKNOWN";
    }
}
