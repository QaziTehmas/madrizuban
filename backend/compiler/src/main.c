#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "interpreter.h"
#include "errors.h"

#define MAX_SRC_SIZE (1024 * 64)

static void print_banner(void) {
    printf("  __  __           _        _  _____       _               \n");
    printf(" |  \\/  |         | |      (_)|__  /      | |              \n");
    printf(" | \\  / | __ _  __| | _ __  _   / / _   _ | |__   __ _ _ __\n");
    printf(" | |\\/| |/ _` |/ _` || '__|| |  / / | | | || '_ \\ / _` | '_ \\\n");
    printf(" | |  | | (_| | (_| || |   | | / /__| |_| || |_) | (_| | | | |\n");
    printf(" |_|  |_|\\__,_|\\__,_||_|   |_|/_____\\__,_||_.__/ \\__,_|_| |_|\n");
    printf(GREEN "         Compiler v1.0  -  Madri Zuban (Mother Tongue)\n" RESET);
    printf(GREEN "         Air University  |  Compiler Construction  |  8th Semester\n" RESET);
    printf("         -----------------------------------------------\n\n");
}

static void print_token_table(const char *src) {
    printf(CYAN "  %-5s  %-20s  %s\n" RESET, "LINE", "TOKEN TYPE", "LEXEME");
    printf("  %-5s  %-20s  %s\n", "-----", "--------------------", "------");
    Lexer l;
    lexer_init(&l, src);
    Token t;
    while ((t = lexer_next(&l)).type != TOK_EOF) {
        printf("  %-5d  %-20s  %s\n", t.line, token_type_str(t.type), t.lexeme);
    }
    printf("\n");
}

static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, RED "[MadriZuban Ghalti] " RESET
                "bhaiyaaaa File nahi khuli → '%s'\n", path);
        return NULL;
    }
    char *buf = malloc(MAX_SRC_SIZE);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, MAX_SRC_SIZE - 1, f);
    buf[n] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char *argv[]) {
    print_banner();

    if (argc < 2) {
        printf("  Istemal:  ./madrizuban <file.mz>\n\n");
        printf("  Misaal:   ./madrizuban tests/test1.mz\n\n");
        printf("  Zaroori keywords:\n");
        printf("    int      →  integer variable\n");
        printf("    string   →  string variable\n");
        printf("    bool     →  boolean variable (sach/jhoot)\n");
        printf("    arzkro   →  print output\n");
        printf("    agar     →  if condition\n");
        printf("    warna    →  else block\n");
        printf("    jabtak   →  while loop\n");
        printf("    sach     →  true\n");
        printf("    jhoot    →  false\n\n");
        return 0;
    }

    const char *filename = argv[1];
    char *src = read_file(filename);
    if (!src) return 1;

    printf(GREEN "  Compiling: %s\n\n" RESET, filename);

    /* ── PHASE 1: Lexical Analysis ──────────────────────────────────── */
        printf("==== PHASE 1: Lexical Analysis (Tokenization) ===="
            "\n\n");
    print_token_table(src);
    printf("  [OK] Lexical Analysis complete\n\n");

    /* ── PHASE 2: Syntax Analysis ───────────────────────────────────── */
        printf("==== PHASE 2: Syntax Analysis (Parsing) ===="
            "\n\n");
    Parser parser;
    parser_init(&parser, src);
    ASTNode *ast = parser_parse(&parser);
    if (parser.had_error) {
        fprintf(stderr, RED "\n  ERROR: Parsing failed. %d error(s).\n\n" RESET,
                parser.error_count);
        ast_free(ast);
        free(src);
        return 1;
    }
    printf("  Abstract Syntax Tree:\n\n");
    ast_print(ast, 2);
    printf("\n  [OK] Parsing complete - AST built\n\n");

    /* ── PHASE 3: Semantic Analysis ─────────────────────────────────── */
        printf("==== PHASE 3: Semantic Analysis ===="
            "\n\n");
    SemanticCtx sem;
    semantic_init(&sem);
    int sem_errs = semantic_analyse(&sem, ast);
    if (sem_errs > 0) {
        fprintf(stderr, RED "\n  ERROR: Semantic analysis failed. %d error(s).\n\n" RESET,
                sem_errs);
        ast_free(ast);
        free(src);
        return 1;
    }
    printf("  [OK] Semantic analysis complete - no errors\n\n");

    /* ── PHASE 4: Intermediate Code (TAC) ───────────────────────────── */
        printf("==== PHASE 4: Intermediate Code Generation (TAC) ===="
            "\n\n");
    CodeGenCtx cg;
    codegen_init(&cg, stdout);
    codegen_generate(&cg, ast);
    printf("\n  [OK] TAC generation complete\n\n");

    /* ── PHASE 5: Execution ─────────────────────────────────────────── */
        printf("==== PHASE 5: Execution (Tree-Walk Interpreter) ===="
            "\n\n");
    printf("  --- Program Output ---\n\n");
    Interpreter interp;
    interp_init(&interp);
    interp_run(&interp, ast);
    printf("\n  --- End of Output ---\n\n");

    if (interp.had_runtime_error) {
        fprintf(stderr, RED "  ERROR: Runtime error occurred - program aborted\n\n" RESET);
        ast_free(ast);
        free(src);
        return 1;
    }

    printf("  [OK] Execution complete\n\n");
    printf("  ===========================================\n");
    printf("  MadriZuban compilation successful!\n");
    printf("  ===========================================\n\n");

    ast_free(ast);
    free(src);
    return 0;
}
