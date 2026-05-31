CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
SRCDIR  = src
SRCS    = $(SRCDIR)/main.c      \
          $(SRCDIR)/lexer.c     \
          $(SRCDIR)/ast.c       \
          $(SRCDIR)/parser.c    \
          $(SRCDIR)/semantic.c  \
          $(SRCDIR)/codegen.c   \
          $(SRCDIR)/interpreter.c \
          $(SRCDIR)/symtable.c  \
          $(SRCDIR)/errors.c
.RECIPEPREFIX := >

ifeq ($(OS),Windows_NT)
TARGET  = madrizuban.exe
RM      = del /F /Q
else
TARGET  = madrizuban
RM      = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRCS)
>$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
>@echo ""
>@echo "  Build successful! Run with:  ./$(TARGET) <file.mz>"
>@echo ""

clean:
>$(RM) $(TARGET)

.PHONY: all clean
