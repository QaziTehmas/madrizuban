╔══════════════════════════════════════════════════════════╗
║           MadriZuban Compiler v1.0                       ║
║      Air University | Compiler Construction              ║
║                8th Semester                              ║
╚══════════════════════════════════════════════════════════╝

GROUP MEMBERS:
  Affan Shafiq    (221394)
  Qazi Tehmas     (221445)
  Abdur Rahman    (221457)
  M.Hammad        (221421)

─────────────────────────────────────────────────────────────
WHAT IS MADRIZUBAN?
─────────────────────────────────────────────────────────────
MadriZuban (Madri Zuban = Mother Tongue) is a custom 
programming language with Roman Urdu keywords. It is a 
complete mini-compiler written in C that implements all 
five phases of compilation.

LANGUAGE KEYWORDS:
  int      → integer type
  string   → string type  
  bool     → boolean type
  sach     → true  (Urdu: سچ)
  jhoot    → false (Urdu: جھوٹ)
  arzkro   → print (Urdu: ارز کرو)
  agar     → if    (Urdu: اگر)
  warna    → else  (Urdu: ورنہ)
  jabtak   → while (Urdu: جب تک)

─────────────────────────────────────────────────────────────
HOW TO BUILD
─────────────────────────────────────────────────────────────
Requirements: gcc (any version), make

  make

This produces the ./madrizuban binary.

─────────────────────────────────────────────────────────────
HOW TO RUN
─────────────────────────────────────────────────────────────
  ./madrizuban <sourcefile.mz>

Example:
  ./madrizuban tests/test1_arithmetic.mz
  ./madrizuban tests/test3_jabtak.mz

─────────────────────────────────────────────────────────────
RUN ALL TESTS
─────────────────────────────────────────────────────────────
  make test

─────────────────────────────────────────────────────────────
PROJECT STRUCTURE
─────────────────────────────────────────────────────────────
  src/
    madrizuban.h   → Header: all types, structs, prototypes
    lexer.c        → Phase 1: Lexical Analysis
    parser.c       → Phase 2: Syntax Analysis (Recursive Descent)
    symtable.c     → Symbol Table (hash-chained)
    semantic.c     → Phase 3: Semantic Analysis
    codegen.c      → Phase 4: TAC Code Generation
    interpreter.c  → Phase 5: Tree-Walk Interpreter
    main.c         → Entry point, phase orchestrator

  tests/
    test1_arithmetic.mz   → Integer arithmetic
    test2_agar_warna.mz   → If/else branching
    test3_jabtak.mz       → While loops, factorial
    test4_bool.mz         → Boolean type (sach/jhoot)
    test5_string.mz       → String type
    test6_nested.mz       → Nested control flow
    test7_errors.mz       → Error detection demo

  Makefile               → Build system
  README.txt             → This file

─────────────────────────────────────────────────────────────
SAMPLE MADRIZUBAN PROGRAM
─────────────────────────────────────────────────────────────
  int x = 10;
  string msg = "Salam Duniya";
  bool flag = sach;

  arzkro msg;

  agar (x > 5) {
      arzkro x;
  } warna {
      arzkro 0;
  }

  jabtak (x > 0) {
      arzkro x;
      x = x - 1;
  }
