/*
 * MadriZuban Compiler v1.0
 * main.c - Entry point and phase orchestrator
 *
 * Air University — Compiler Construction (8th Semester)
 * Group: Affan Shafiq (221394), Qazi Tehmas (221445),
 *        Abdur Rahman (221457), M.Hammad (221421)
 *
 * Usage:  ./madrizuban <source.mz>
 */

#include "madrizuban.h"

/* ─── ASCII Banner ────────────────────────────────────────────────────────── */
static void print_banner(void) {
    printf(CYAN);
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║        __  __           _      _  _____       _         ║\n");
    printf("║       |  \\/  | __ _  __| |_ __(_)|__  /_   _| |__      ║\n");
    printf("║       | |\\/| |/ _` |/ _` | '__| |  / /| | | | '_ \\     ║\n");
    printf("║       | |  | | (_| | (_| | |  | | / /_| |_| | |_) |    ║\n");
    printf("║       |_|  |_|\\__,_|\\__,_|_|  |_|/____\\__,_|_.__/     ║\n");
    printf("║                                                          ║\n");
    printf("║            Compiler v1.0  —  Madri Zuban                ║\n");
    printf("║       Air University  |  Compiler Construction          ║\n");
    printf("║                   8th Semester                           ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf(RESET "\n");
}

/* ─── Keyword Legend ──────────────────────────────────────────────────────── */
static void print_legend(void) {
    printf(BOLD "  MadriZuban Keyword Legend:\n" RESET);
    printf("  %-12s → %s\n", "int",    "integer variable type");
    printf("  %-12s → %s\n", "string", "string variable type");
    printf("  %-12s → %s\n", "bool",   "boolean variable type");
    printf("  %-12s → %s\n", "sach",   "true  (Urdu: سچ)");
    printf("  %-12s → %s\n", "jhoot",  "false (Urdu: جھوٹ)");
    printf("  %-12s → %s\n", "arzkro", "print output (Urdu: ارز کرو)");
    printf("  %-12s → %s\n", "agar",   "if    (Urdu: اگر)");
    printf("  %-12s → %s\n", "warna",  "else  (Urdu: ورنہ)");
    printf("  %-12s → %s\n", "jabtak", "while (Urdu: جب تک)");
    printf("\n");
}

/* ─── Phase header ────────────────────────────────────────────────────────── */
static void phase(int num, const char *name) {
    printf(BOLD GREEN "\n── Phase %d: %s ──\n" RESET, num, name);
}

/* ─── Read file ───────────────────────────────────────────────────────────── */
static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, RED "[MadriZuban Ghalti]" RESET
                " bhaiyaaaa File nahi mili: '%s'\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char *buf = (char *)malloc(sz + 1);
    if (!buf) { fclose(f); exit(1); }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    return buf;
}

/* ─── Main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    /* Flush stderr before printing banner for clean output ordering */
    fflush(stderr);
    print_banner();

    if (argc < 2) {
        print_legend();
        fprintf(stderr, RED "Usage:" RESET " ./madrizuban <source.mz>\n\n");
        fprintf(stderr, "Example:  ./madrizuban tests/test1.mz\n\n");
        return 1;
    }

    const char *filename = argv[1];
    printf(BOLD "  Source file: " RESET "%s\n\n", filename);

    /* ── Read source ── */
    char *source = read_file(filename);

    /* ───────────────────────────────────────────────
       PHASE 1: Lexical Analysis
    ─────────────────────────────────────────────── */
    phase(1, "Lexical Analysis  (Lexer)");
    lexer_init(source);

    /* Print all tokens */
    printf("  %-5s %-18s %s\n", "Line", "Token Type", "Lexeme");
    printf("  %-5s %-18s %s\n", "----", "----------", "------");

    /* We need two passes: one to show tokens, one to actually parse */
    /* So we'll lex again inside the parser; here just show token list */
    lexer_init(source);
    Token t;
    int tok_count = 0;
    do {
        t = lexer_next();
        printf("  %-5d %-18s '%s'\n", t.line, token_type_str(t.type), t.lexeme);
        tok_count++;
    } while (t.type != TOK_EOF);
    printf(GREEN "  → %d tokens erkened.\n" RESET, tok_count);

    /* ───────────────────────────────────────────────
       PHASE 2: Syntax Analysis (Parser)
    ─────────────────────────────────────────────── */
    phase(2, "Syntax Analysis   (Parser)");
    lexer_init(source); /* re-init lexer for parser */
    ASTNode *ast = parser_parse();
    if (!ast) {
        fprintf(stderr, RED "  Parse failed — aborting.\n" RESET);
        free(source);
        return 1;
    }
    printf(GREEN "  → Parse successful. AST built.\n\n" RESET);
    printf("  Abstract Syntax Tree:\n");
    ast_print(ast, 2);

    /* ───────────────────────────────────────────────
       PHASE 3: Semantic Analysis
    ─────────────────────────────────────────────── */
    phase(3, "Semantic Analysis");
    SymTable *symtab = symtable_create();
    int sem_ok = semantic_check(ast, symtab);
    if (!sem_ok) {
        fprintf(stderr, RED "  Semantic errors found — aborting.\n" RESET);
        ast_free(ast);
        symtable_destroy(symtab);
        free(source);
        return 1;
    }
    printf(GREEN "  → Semantic analysis clean.\n" RESET);
    symtable_print(symtab);

    /* ───────────────────────────────────────────────
       PHASE 4: Intermediate Code Generation (TAC)
    ─────────────────────────────────────────────── */
    phase(4, "Intermediate Code Generation  (TAC)");
    TACInstr *tac = codegen_generate(ast);
    tac_print(tac);
    printf(GREEN "  → TAC generation complete.\n" RESET);

    /* ───────────────────────────────────────────────
       PHASE 5: Execution (Tree-Walk Interpreter)
    ─────────────────────────────────────────────── */
    phase(5, "Execution  (Interpreter)");
    printf(BOLD "  Program Output:\n" RESET);
    printf("  " CYAN "─────────────────────\n" RESET);

    /* Fresh symbol table for execution (semantic one has no values) */
    SymTable *exec_st = symtable_create();
    interpreter_run(ast, exec_st);

    printf("  " CYAN "─────────────────────\n" RESET);
    printf(GREEN "  → Execution complete.\n\n" RESET);

    /* ── Cleanup ── */
    tac_free(tac);
    ast_free(ast);
    symtable_destroy(symtab);
    symtable_destroy(exec_st);
    free(source);

    printf(BOLD CYAN "  MadriZuban compilation + execution finished.\n" RESET "\n");
    return 0;
}
