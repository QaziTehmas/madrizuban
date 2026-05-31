# MadriZuban Compiler - Makefile
# Air University | Compiler Construction | 8th Semester
#
# Usage:
#   make          → build the compiler
#   make test     → run all test programs
#   make clean    → remove build artifacts

CC      = gcc
CFLAGS  = -Wall -Wextra -g -std=c99 -pedantic
SRCS    = src/main.c src/lexer.c src/parser.c src/symtable.c \
		  src/semantic.c src/codegen.c src/interpreter.c

# Produce .exe on Windows (when using MSYS2/MinGW or native Windows toolchain)
EXE :=
ifeq ($(OS),Windows_NT)
EXE := .exe
endif

TARGET  = madrizuban$(EXE)

all: $(TARGET)

$(TARGET): $(SRCS) src/madrizuban.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)
	@echo "✓ MadriZuban compiler built successfully!"

test: $(TARGET)
	@echo ""
	@echo "══════════════════════════════════════════════"
	@echo "  Running MadriZuban Test Suite"
	@echo "══════════════════════════════════════════════"
	@echo ""
	@echo "── Test 1: Arithmetic ──────────────────────────"
	./$(TARGET) tests/test1_arithmetic.mz
	@echo ""
	@echo "── Test 2: If/Else (agar/warna) ───────────────"
	./$(TARGET) tests/test2_agar_warna.mz
	@echo ""
	@echo "── Test 3: While Loop (jabtak) ─────────────────"
	./$(TARGET) tests/test3_jabtak.mz
	@echo ""
	@echo "── Test 4: Bool Type (sach/jhoot) ─────────────"
	./$(TARGET) tests/test4_bool.mz
	@echo ""
	@echo "── Test 5: String Type ─────────────────────────"
	./$(TARGET) tests/test5_string.mz
	@echo ""
	@echo "── Test 6: Nested if/else ──────────────────────"
	./$(TARGET) tests/test6_nested.mz
	@echo ""
	@echo "── Test 7: Error Handling (expect errors) ──────"
	./$(TARGET) tests/test7_errors.mz || true
	@echo ""
	@echo "══════════════════════════════════════════════"
	@echo "  All tests complete!"
	@echo "══════════════════════════════════════════════"

clean:
	rm -f $(TARGET) *.o

.PHONY: all test clean
