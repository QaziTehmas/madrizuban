#ifndef ERRORS_H
#define ERRORS_H

/* Colour codes for terminal
	Disabled by default to avoid ANSI escape sequences and
	Unicode box-drawing characters in environments that don't
	render them correctly. Set these back to ANSI escapes if
	you want colored output. */
#define RED     ""
#define GREEN   ""
#define YELLOW  ""
#define CYAN    ""
#define RESET   ""

/* Error categories */
#define ERR_LEXER    "MadriZuban Ghalti [Lexer]"
#define ERR_PARSER   "MadriZuban Ghalti [Parser]"
#define ERR_SEMANTIC "MadriZuban Ghalti [Semantic]"
#define ERR_RUNTIME  "MadriZuban Ghalti [Runtime]"
#define WARN_TAG     "MadriZuban Khabardar"

void lex_error(int line, const char *msg, const char *token);
void parse_error(int line, const char *msg, const char *token);
void semantic_error(int line, const char *msg, const char *detail);
void semantic_warning(int line, const char *msg, const char *detail);
void runtime_error(int line, const char *msg, const char *detail);

#endif
