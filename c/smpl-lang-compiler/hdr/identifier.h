#ifndef _IDENTIFIER_H_
#define _IDENTIFIER_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

#include <graphviz/cgraph.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_VAR_NAME_LEN
#define MAX_VAR_NAME_LEN 1<<4 //16
#endif//MAX_VAR_NAME_LEN

struct AstNode;

struct Ident {
	char *name;
};

struct Ident *
new_ident ();

struct Ident *
deep_copy_ident (struct Ident *src);

void
create_ident_agedge_set (char *label,
												 int len,
												 struct AstNode *n);

int
interpret_identifier (struct AstNode *n,
											struct InterpreterCtx *ictx);

struct Operand *
compile_identifier (struct AstNode *n,
										struct CompilerCtx *cctx);

void
free_identifier (struct AstNode **n);

#endif//_IDENTIFIER_H_
