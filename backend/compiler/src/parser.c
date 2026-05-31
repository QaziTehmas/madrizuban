/*
 * MadriZuban Compiler v1.0
 * parser.c - Recursive Descent Parser
 *
 * Grammar (CFG):
 *
 *  program      → stmt*
 *  stmt         → var_decl | assign_stmt | arzkro_stmt
 *               | agar_stmt | jabtak_stmt | block
 *  var_decl     → type IDENT '=' expr ';'
 *  assign_stmt  → IDENT '=' expr ';'
 *  arzkro_stmt  → 'arzkro' expr ';'
 *  agar_stmt    → 'agar' '(' expr ')' block ( 'warna' block )?
 *  jabtak_stmt  → 'jabtak' '(' expr ')' block
 *  block        → '{' stmt* '}'
 *  expr         → comparison
 *  comparison   → addition ( ('=='|'!='|'<'|'>'|'<='|'>=') addition )*
 *  addition     → term ( ('+'|'-') term )*
 *  term         → unary ( ('*'|'/') unary )*
 *  unary        → '-' unary | primary
 *  primary      → INT_LIT | STRING_LIT | 'sach' | 'jhoot' | IDENT | '(' expr ')'
 *  type         → 'int' | 'string' | 'bool'
 */

#include "madrizuban.h"

/* ─── Parser state ────────────────────────────────────────────────────────── */
static Token cur_tok;
static int   error_count = 0;

/* ─── Helpers ─────────────────────────────────────────────────────────────── */
static void advance(void) {
    cur_tok = lexer_next();
}

static Token peek(void) {
    return lexer_peek();
}

static void parse_error(const char *msg) {
    fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
            " bhaiyaaaa Line %d: %s (mila: '%s')\n",
            cur_tok.line, msg, cur_tok.lexeme);
    error_count++;
}

static int expect(TokenType t) {
    if (cur_tok.type == t) {
        advance();
        return 1;
    }
    char buf[128];
    snprintf(buf, sizeof(buf),
             "'%s' chahiye tha yahan", token_type_str(t));
    parse_error(buf);
    return 0;
}

static ASTNode *new_node(NodeType type, int line) {
    ASTNode *n = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    n->type = type;
    n->line = line;
    return n;
}

/* ─── Forward declarations ────────────────────────────────────────────────── */
static ASTNode *parse_stmt(void);
static ASTNode *parse_expr(void);
static ASTNode *parse_block(void);

/* ─── Primary ─────────────────────────────────────────────────────────────── */
static ASTNode *parse_primary(void) {
    int line = cur_tok.line;

    /* Integer literal */
    if (cur_tok.type == TOK_INT_LIT) {
        ASTNode *n = new_node(NODE_INT_LIT, line);
        n->ival = atoi(cur_tok.lexeme);
        advance();
        return n;
    }

    /* String literal */
    if (cur_tok.type == TOK_STRING_LIT) {
        ASTNode *n = new_node(NODE_STRING_LIT, line);
        strncpy(n->sval, cur_tok.lexeme, 255);
        advance();
        return n;
    }

    /* sach (true) */
    if (cur_tok.type == TOK_SACH) {
        ASTNode *n = new_node(NODE_BOOL_LIT, line);
        n->bval = 1;
        strcpy(n->sval, "sach");
        advance();
        return n;
    }

    /* jhoot (false) */
    if (cur_tok.type == TOK_JHOOT) {
        ASTNode *n = new_node(NODE_BOOL_LIT, line);
        n->bval = 0;
        strcpy(n->sval, "jhoot");
        advance();
        return n;
    }

    /* Identifier */
    if (cur_tok.type == TOK_IDENT) {
        ASTNode *n = new_node(NODE_IDENT, line);
        strncpy(n->sval, cur_tok.lexeme, 255);
        advance();
        return n;
    }

    /* Parenthesised expression */
    if (cur_tok.type == TOK_LPAREN) {
        advance(); /* consume '(' */
        ASTNode *n = parse_expr();
        expect(TOK_RPAREN);
        return n;
    }

    parse_error("Yeh token yahan nahi chahiye tha");
    advance(); /* recover */
    return NULL;
}

/* ─── Unary ───────────────────────────────────────────────────────────────── */
static ASTNode *parse_unary(void) {
    if (cur_tok.type == TOK_MINUS) {
        int line = cur_tok.line;
        advance();
        ASTNode *n = new_node(NODE_BINOP, line);
        strcpy(n->op, "-");
        n->left  = new_node(NODE_INT_LIT, line); /* 0 - x */
        n->left->ival = 0;
        n->right = parse_unary();
        return n;
    }
    return parse_primary();
}

/* ─── Term (* /) ──────────────────────────────────────────────────────────── */
static ASTNode *parse_term(void) {
    ASTNode *left = parse_unary();
    while (cur_tok.type == TOK_STAR || cur_tok.type == TOK_SLASH) {
        int line = cur_tok.line;
        char op[4];
        strcpy(op, cur_tok.lexeme);
        advance();
        ASTNode *n = new_node(NODE_BINOP, line);
        strcpy(n->op, op);
        n->left  = left;
        n->right = parse_unary();
        left = n;
    }
    return left;
}

/* ─── Addition (+ -) ──────────────────────────────────────────────────────── */
static ASTNode *parse_addition(void) {
    ASTNode *left = parse_term();
    while (cur_tok.type == TOK_PLUS || cur_tok.type == TOK_MINUS) {
        int line = cur_tok.line;
        char op[4];
        strcpy(op, cur_tok.lexeme);
        advance();
        ASTNode *n = new_node(NODE_BINOP, line);
        strcpy(n->op, op);
        n->left  = left;
        n->right = parse_term();
        left = n;
    }
    return left;
}

/* ─── Comparison (== != < > <= >=) ───────────────────────────────────────── */
static ASTNode *parse_comparison(void) {
    ASTNode *left = parse_addition();
    while (cur_tok.type == TOK_EQ  || cur_tok.type == TOK_NEQ ||
           cur_tok.type == TOK_LT  || cur_tok.type == TOK_GT  ||
           cur_tok.type == TOK_LTE || cur_tok.type == TOK_GTE) {
        int line = cur_tok.line;
        char op[4];
        strcpy(op, cur_tok.lexeme);
        advance();
        ASTNode *n = new_node(NODE_BINOP, line);
        strcpy(n->op, op);
        n->left  = left;
        n->right = parse_addition();
        left = n;
    }
    return left;
}

/* ─── Expression ──────────────────────────────────────────────────────────── */
static ASTNode *parse_expr(void) {
    return parse_comparison();
}

/* ─── Block { stmts } ─────────────────────────────────────────────────────── */
static ASTNode *parse_block(void) {
    int line = cur_tok.line;
    expect(TOK_LBRACE);

    ASTNode *block = new_node(NODE_BLOCK, line);
    ASTNode *tail  = NULL;

    while (cur_tok.type != TOK_RBRACE && cur_tok.type != TOK_EOF) {
        ASTNode *s = parse_stmt();
        if (!s) continue;
        if (!block->body) {
            block->body = s;
            tail = s;
        } else {
            tail->next = s;
            tail = s;
        }
    }
    expect(TOK_RBRACE);
    return block;
}

/* ─── Statement ───────────────────────────────────────────────────────────── */
static ASTNode *parse_stmt(void) {
    int line = cur_tok.line;

    /* Variable declaration: int/string/bool IDENT = expr ; */
    if (cur_tok.type == TOK_INT || cur_tok.type == TOK_STRING || cur_tok.type == TOK_BOOL) {
        DataType dt = (cur_tok.type == TOK_INT)    ? TYPE_INT :
                      (cur_tok.type == TOK_STRING)  ? TYPE_STRING : TYPE_BOOL;
        advance(); /* consume type */

        ASTNode *n = new_node(NODE_VAR_DECL, line);
        n->dtype = dt;

        if (cur_tok.type != TOK_IDENT) {
            parse_error("Variable ka naam chahiye tha");
            return NULL;
        }
        strncpy(n->sval, cur_tok.lexeme, 255);
        advance();

        expect(TOK_ASSIGN);
        n->left = parse_expr();
        expect(TOK_SEMICOLON);
        return n;
    }

    /* arzkro expr ; */
    if (cur_tok.type == TOK_ARZKRO) {
        advance();
        ASTNode *n = new_node(NODE_ARZKRO, line);
        n->left = parse_expr();
        expect(TOK_SEMICOLON);
        return n;
    }

    /* agar ( expr ) block [ warna block ] */
    if (cur_tok.type == TOK_AGAR) {
        advance();
        ASTNode *n = new_node(NODE_AGAR, line);
        expect(TOK_LPAREN);
        n->cond = parse_expr();
        expect(TOK_RPAREN);
        n->body = parse_block();
        if (cur_tok.type == TOK_WARNA) {
            advance();
            n->elseb = parse_block();
        }
        return n;
    }

    /* jabtak ( expr ) block */
    if (cur_tok.type == TOK_JABTAK) {
        advance();
        ASTNode *n = new_node(NODE_JABTAK, line);
        expect(TOK_LPAREN);
        n->cond = parse_expr();
        expect(TOK_RPAREN);
        n->body = parse_block();
        return n;
    }

    /* Assignment: IDENT = expr ; */
    if (cur_tok.type == TOK_IDENT) {
        char name[256];
        strncpy(name, cur_tok.lexeme, 255);
        advance();

        if (cur_tok.type == TOK_ASSIGN) {
            advance();
            ASTNode *n = new_node(NODE_ASSIGN, line);
            strncpy(n->sval, name, 255);
            n->left = parse_expr();
            expect(TOK_SEMICOLON);
            return n;
        }

        parse_error("'=' chahiye tha assignment ke liye");
        return NULL;
    }

    /* Block */
    if (cur_tok.type == TOK_LBRACE)
        return parse_block();

    /* Unknown — skip */
    if (cur_tok.type != TOK_EOF) {
        parse_error("Yeh statement nahi samajh aaya");
        advance();
    }
    return NULL;
}

/* ─── Parse entry point ───────────────────────────────────────────────────── */
ASTNode *parser_parse(void) {
    advance(); /* prime the pump */

    ASTNode *root = new_node(NODE_PROGRAM, 1);
    ASTNode *tail = NULL;

    while (cur_tok.type != TOK_EOF) {
        ASTNode *s = parse_stmt();
        if (!s) continue;
        if (!root->body) {
            root->body = s;
            tail = s;
        } else {
            tail->next = s;
            tail = s;
        }
    }

    if (error_count > 0) {
        fprintf(stderr, RED "[MadriZuban]" RESET
                " %d parse ghalti(yan) mili — compilation rok di gayi.\n",
                error_count);
    }
    return root;
}

/* ─── AST pretty-printer ──────────────────────────────────────────────────── */
static const char *node_type_str(NodeType t) {
    switch (t) {
        case NODE_PROGRAM:    return "PROGRAM";
        case NODE_VAR_DECL:   return "VAR_DECL";
        case NODE_ASSIGN:     return "ASSIGN";
        case NODE_ARZKRO:     return "ARZKRO";
        case NODE_AGAR:       return "AGAR";
        case NODE_JABTAK:     return "JABTAK";
        case NODE_BLOCK:      return "BLOCK";
        case NODE_BINOP:      return "BINOP";
        case NODE_IDENT:      return "IDENT";
        case NODE_INT_LIT:    return "INT_LIT";
        case NODE_STRING_LIT: return "STRING_LIT";
        case NODE_BOOL_LIT:   return "BOOL_LIT";
        default:              return "?";
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");

    printf(CYAN "[%s]" RESET, node_type_str(node->type));

    switch (node->type) {
        case NODE_INT_LIT:    printf(" %d",   node->ival);  break;
        case NODE_STRING_LIT: printf(" \"%s\"", node->sval); break;
        case NODE_BOOL_LIT:   printf(" %s",   node->sval);  break;
        case NODE_IDENT:      printf(" '%s'", node->sval);  break;
        case NODE_VAR_DECL:   printf(" %s:%s", node->sval, dtype_str(node->dtype)); break;
        case NODE_ASSIGN:     printf(" '%s'", node->sval);  break;
        case NODE_BINOP:      printf(" '%s'", node->op);    break;
        default: break;
    }
    printf("\n");

    ast_print(node->cond,  indent + 1);
    ast_print(node->left,  indent + 1);
    ast_print(node->right, indent + 1);
    ast_print(node->body,  indent + 1);
    ast_print(node->elseb, indent + 1);

    /* Print sibling list */
    if (node->next) ast_print(node->next, indent);
}

/* ─── AST free ────────────────────────────────────────────────────────────── */
void ast_free(ASTNode *node) {
    if (!node) return;
    ast_free(node->left);
    ast_free(node->right);
    ast_free(node->cond);
    ast_free(node->body);
    ast_free(node->elseb);
    ast_free(node->next);
    free(node);
}
