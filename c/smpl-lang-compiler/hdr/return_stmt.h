#ifndef _RETURN_STMT_H_
#define _RETURN_STMT_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

struct ReturnStmt {
	struct AstNode *ret_val;
};

struct ReturnStmt *
new_return_stmt ();

struct ReturnStmt *
deep_copy_return_stmt (struct ReturnStmt *src);

void
create_return_stmt_agedge_set (char *label,
															 int len,
															 struct AstNode *n);

int
interpret_return_stmt (struct AstNode *n,
											 struct InterpreterCtx *ictx);

struct Operand *
compile_return_stmt (struct AstNode *n,
										 struct CompilerCtx *cctx);

void
free_return_stmt (struct AstNode **n);

#endif//_RETURN_STMT_H_
