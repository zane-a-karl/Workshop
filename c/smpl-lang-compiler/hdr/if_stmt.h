#ifndef _IF_STMT_H_
#define _IF_STMT_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

struct IfStmt {
	struct AstNode     *condition;
	struct AstNodeList *then_stmts;
	struct AstNodeList *else_stmts;
};

struct IfStmt *
new_if_stmt ();

struct IfStmt *
deep_copy_if_stmt (struct IfStmt *src);

void
create_if_stmt_agedge_set (char *label,
													 int len,
													 struct AstNode *n);

int
interpret_if_stmt (struct AstNode *n,
									 struct InterpreterCtx *ictx);

struct Operand *
compile_if_stmt (struct AstNode *n,
								 struct CompilerCtx *cctx);

void
free_if_stmt (struct AstNode **n);

#endif//_IF_STMT_H_
