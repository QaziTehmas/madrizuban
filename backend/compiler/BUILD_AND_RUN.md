# Build & Run — MadriZuban Compiler

This document shows how to compile and run the MadriZuban compiler on Windows (MSYS2 / Git Bash / MinGW / PowerShell) and Linux/WSL.

**Prerequisites**
- `gcc` (or any C compiler that supports C99)
- `make` (optional)

**Files of interest**
- Entry point: [src/main.c](src/main.c)
- Color/Unicode macros: [src/errors.h](src/errors.h)

**Build (recommended: from project root)**

1. Using `make` (if available):
```bash
make
```
This produces the executable `madrizuban` (or `madrizuban.exe` on Windows).

2. Without `make` (direct `gcc`):
```bash
gcc -Wall -Wextra -std=c99 -g -o madrizuban src/main.c src/lexer.c src/ast.c src/parser.c src/semantic.c src/codegen.c src/interpreter.c src/symtable.c src/errors.c
```

**Run**
- Run a test or your `.mz` program from the project root:
```bash
# On Windows (PowerShell/CMD/Git Bash)
./madrizuban.exe tests/test1_arithmetic.mz

# On Linux/WSL
./madrizuban tests/test1_arithmetic.mz
```

**Useful test files**
- `tests/test1_arithmetic.mz`
- `tests/test2_agar_warna.mz`
- `tests/test3_jabtak.mz`
- `tests/test4_types.mz`
- `tests/test5_factorial.mz`
- `tests/test6_errors.mz`

**If you see garbled box-drawing or strange characters**
- The program prints Unicode box-drawing characters and ANSI color escapes by default. If your terminal doesn't use UTF‑8 or doesn't show those glyphs, you might see mojibake.
- Quick fixes:
  - Use a UTF‑8 terminal (Windows Terminal, WSL bash, or configure Git Bash to use `en_US.UTF-8`).
  - In PowerShell before running:
```powershell
chcp 65001
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
.\madrizuban.exe smoke.mz
```
- To permanently disable colored/Unicode output: edit `[src/errors.h](src/errors.h)` and set the color macros to empty strings (this repository already provides an ASCII-friendly variant).

**Re-enable fancy output (if desired)**
- Restore ANSI escape sequences in `[src/errors.h](src/errors.h)`:
```c
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"
```
- Rebuild with `make` or the `gcc` command above.

**Troubleshooting**
- `make` not found on Windows: install MSYS2 or use `mingw-w64` / Git for Windows which include `make`, or compile with the `gcc` command above.
- Compilation errors: ensure all `src/*.c` files are present and that your compiler supports C99.

If you'd like, I can add a `Makefile` rule or a build script for Windows PowerShell to automate the `gcc` compile command. Want me to add that?