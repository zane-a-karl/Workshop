#ifndef _LEXER_H_
#define _LEXER_H_

#include "../hdr/token.h"

#include <regex.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_TKN_LEN
#define MAX_TKN_LEN 64
#endif//MAX_TKN_LEN

struct Lexer {
	FILE *fin;
	struct TokenList *tl;
	char buf[MAX_TKN_LEN];
	int pos;     // current position within `buf`
	int tkn_num; // total # of tkns in list or current tkn #
	int line;    // Within the file we're currently on
	int col;     // Within the file we're currently on
};

struct Lexer *
new_lexer (char *input_filename);

void
free_lexer (struct Lexer **lxr);

struct TokenNode *
lex_next_tkn (struct Lexer *lxr);

void
check_ferror (FILE *fin);

bool
triggered_feof (FILE *fin);

bool
found_whitespace (char ch);

bool
found_newline (char ch,
							 struct Lexer *l);

bool
can_create_alnum_token (char *buf,
												regex_t *re_alnum);

bool
is_a_possible_individual_symbol (char c);

bool
is_a_possible_symbol (char *buf);

bool
can_create_symbol_token (char *buf);

#endif//_LEXER_H_
