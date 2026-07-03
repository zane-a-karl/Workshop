#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_TKN_LEN
#define MAX_TKN_LEN 64
#endif//MAX_TKN_LEN

enum TokenType {
	IDENT,
	NUMBER,
	OP_INEQ,
	OP_EQ,
	OP_LT,
	OP_LE,
	OP_GT,
	OP_GE,
	LBRACKET,
	RBRACKET,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	ASTERISK,
	SLASH,
	PLUS,
	MINUS,
	LET,
	LARROW,
	CALL,
	IF,
	THEN,
	ELSE,
	FI,
	WHILE,
	DO,
	OD,
	RETURN,
	VAR,
	ARRAY,
	SEMICOLON,
	VOID,
	FUNCTION,
	MAIN,
	PERIOD,
	COMMA,
};

struct Token {
	enum TokenType type;
	char *raw;// raw token string
	int val;  // just for #s, left unused for others
	int line; // line # on which it was parsed
};

struct TokenNode {
	struct Token *tkn;
	struct TokenNode *next;
};

struct TokenList {
	struct TokenNode *head;
};

struct Token *
new_token ();

struct TokenNode *
new_token_node (struct Token *t);

struct TokenList *
new_token_list ();

void
next_token (struct TokenNode **tn);

struct TokenNode *
push_token (struct TokenList *tl,
						struct Token *t);

void
print_token_list (struct TokenList *tl);

void
free_token (struct Token **t);

void
free_token_node (struct TokenNode **tn);

void
free_token_list (struct TokenList **tl);

struct Token *
new_alnum_token (char *buf,
								 int line);

struct Token *
new_symbol_token (char *buf,
									int line);

void
check_regex_compilation (int rv);

#endif//_TOKEN_H_
