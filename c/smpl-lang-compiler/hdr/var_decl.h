#ifndef _VAR_DECL_H_
#define _VAR_DECL_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

#include <stdlib.h>

struct VarDecl {
	struct AstNode     *ident; //Identifier
	struct AstNodeList *dims;  //Dimensions
};

struct VarDecl *
new_vd ();

struct VarDecl *
deep_copy_var_decl (struct VarDecl *src);

void
create_var_decl_agedge_set (char *label,
														int len,
														struct AstNode *n);

int
interpret_var_decl (struct AstNode *n,
										struct InterpreterCtx *ictx);

struct Operand *
compile_var_decl (struct AstNode *n,
									struct CompilerCtx *cctx);

void
free_var_decl (struct AstNode **n);

#endif//_VAR_DECL_H_
