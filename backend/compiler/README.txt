======================================================
  MadriZuban Compiler v1.0
  Air University | Compiler Construction | 8th Semester
======================================================

Group Members:
  1. Affan Shafiq      (221394)
  2. Qazi Tehmas       (221445)
  3. Abdur Rahman      (221457)
  4. M. Hammad         (221421)

------------------------------------------------------
REQUIREMENTS
------------------------------------------------------
  - GCC compiler  (sudo apt install gcc  OR  mingw on Windows)
  - GNU Make       (sudo apt install make)

------------------------------------------------------
HOW TO COMPILE THE COMPILER
------------------------------------------------------
  1. Open a terminal in this folder
  2. Run:   make
  3. This produces the executable:  ./madrizuban

------------------------------------------------------
HOW TO RUN A PROGRAM
------------------------------------------------------
  ./madrizuban tests/test1_arithmetic.mz
  ./madrizuban tests/test2_agar_warna.mz
  ./madrizuban tests/test3_jabtak.mz
  ./madrizuban tests/test4_types.mz
  ./madrizuban tests/test5_factorial.mz
  ./madrizuban tests/test6_errors.mz     <- shows error handling

  You can also write your own .mz file and run it!

------------------------------------------------------
MADRIZUBAN LANGUAGE KEYWORD REFERENCE
------------------------------------------------------
  Keyword    Meaning
  -------    -------
  int        Declare an integer variable
  string     Declare a string variable
  bool       Declare a boolean variable
  sach       Boolean true  ("truth")
  jhoot      Boolean false ("lie")
  arzkro     Print output  ("arz karo" = present/say)
  agar       If condition  ("agar" = if)
  warna      Else block    ("warna" = otherwise)
  jabtak     While loop    ("jabtak" = until/while)

------------------------------------------------------
SAMPLE CODE
------------------------------------------------------
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

------------------------------------------------------
PROJECT STRUCTURE
------------------------------------------------------
  src/
    main.c         - Entry point, phase output, banner
    lexer.c/h      - Tokenizer (Lexical Analysis)
    ast.c/h        - Abstract Syntax Tree nodes
    parser.c/h     - Recursive-Descent Parser
    semantic.c/h   - Semantic Analysis + Type Checking
    codegen.c/h    - Three Address Code (TAC) Generation
    interpreter.c/h- Tree-Walk Interpreter (Execution)
    symtable.c/h   - Symbol Table
    errors.c/h     - Roman Urdu Error Messages

  tests/
    test1_arithmetic.mz   - Basic arithmetic
    test2_agar_warna.mz   - If/Else
    test3_jabtak.mz       - While loop
    test4_types.mz        - String and Bool types
    test5_factorial.mz    - Factorial (5! = 120)
    test6_errors.mz       - Error handling demo

======================================================
