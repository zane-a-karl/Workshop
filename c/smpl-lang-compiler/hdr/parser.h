/**
 * The EBNF for the respective morpheme is located above each
 * function sharing the same name as the morpheme. I include
 * the "alphabet"-like terms here for convenience
 *  as they have already been taken into account during lexing.
 * letter = “a”|“b”|...|“z”.
 * digit = “0”|“1”|...|“9”.
 * relOp = “==“|“!=“|“<“|“<=“|“>“|“>=“.
 * ident = letter {letter | digit}.
 * number = digit {digit}.
 */
#ifndef _PARSER_H_
#define _PARSER_H_

#include "../hdr/utils.h"
#include "../hdr/token.h"
#include "../hdr/lexer.h"
#include "../hdr/ast.h"

#include <assert.h>
#include <ctype.h>
#include <graphviz/cgraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_NUM_VARS
#define MAX_NUM_VARS 1<<8
#endif//MAX_NUM_VARS

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN 1<<4
#endif//MAX_NAME_LEN

#ifndef MAX_LINE_LEN
#define MAX_LINE_LEN 1<<8
#endif//MAX_LINE_LEN

enum ParserSector {
	COMPUTATION,
	VAR_DECL,
	TYPE_DECL,
	FUNC_DECL,
	FORMAL_PARAM,
	FUNC_BODY,
	STAT_SEQ,
	STMT,
	ASSIGNMENT,
	DESIGNATOR,
	EXPRESSION,
	TERM,
	FACTOR,
	FUNC_CALL,
	IF_STMT,
	RELATION,
	WHILE_STMT,
	RETURN_STMT
};

struct Parser {
	struct Lexer *lxr;
	struct TokenNode *curr_tkn; // pointer to most recently lexed token
	FILE *fwarn;
};

struct Parser *
new_parser (char *filename);

void
free_parser (struct Parser **p);

bool
peek_tkn (enum TokenType t,
					struct Parser *p);

bool
peek_stmt_initials (struct Parser *p);

bool
peek_fctr_initials (struct Parser *p);

void
throw_parser_error (enum TokenType t,
										struct Parser *p,
										enum ParserSector ps);

void
throw_parser_warning (enum TokenType t,
											struct Parser *p,
											enum ParserSector ps);

void
consume_tkn (enum TokenType t,
						 struct Parser *p,
						 enum ParserSector ps);

struct Ast *
parse (struct Parser *p,
			 Agraph_t      *tlg);

struct AstNode *
smpl_computation (struct Parser *p,
									Agraph_t      *g);

struct AstNodeList *
smpl_var_decl (struct Parser *p,
							 Agraph_t      *g);

struct AstNodeList *
smpl_type_decl (struct Parser *p,
								Agraph_t      *g);

struct AstNode *
smpl_number (struct Parser *p,
						 Agraph_t      *g,
						 enum ParserSector ps);

struct AstNode *
smpl_ident (struct Parser *p,
						Agraph_t      *g,
						enum ParserSector ps);

struct AstNode *
smpl_func_decl (struct Parser *p,
								Agraph_t      *g);

struct AstNodeList *
smpl_formal_param (struct Parser *p,
									 Agraph_t      *g);

struct FuncBody *
smpl_func_body (struct Parser *p,
								Agraph_t      *g);

struct AstNodeList *
smpl_stat_sequence (struct Parser *p,
										Agraph_t      *g);

struct AstNode *
smpl_statement (struct Parser *p,
								Agraph_t      *g);

struct AstNode *
smpl_assignment (struct Parser *p,
								 Agraph_t      *g);

struct AstNode *
smpl_designator (struct Parser *p,
								 Agraph_t      *g);

struct AstNode *
smpl_expression (struct Parser *p,
								 Agraph_t      *g);

struct AstNode *
smpl_term (struct Parser *p,
					 Agraph_t      *g);

struct AstNode *
smpl_factor (struct Parser *p,
						 Agraph_t      *g);

struct AstNode *
smpl_func_call (struct Parser *p,
								Agraph_t      *g);

struct AstNode *
smpl_relation (struct Parser *p,
							 Agraph_t      *g);

struct AstNode *
smpl_if_statement (struct Parser *p,
									 Agraph_t      *g);

struct AstNode *
smpl_while_statement (struct Parser *p,
											Agraph_t      *g);

struct AstNode *
smpl_return_statement (struct Parser *p,
											 Agraph_t      *g);

/* int */
/* val (char *c); */

/* int */
/* lookup (VAR vt[MAX_NUM_VARS], */
/* 				char *name); */

#endif // _PARSER_H_
