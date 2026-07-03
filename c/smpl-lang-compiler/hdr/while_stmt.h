#ifndef _WHILE_STMT_H_
#define _WHILE_STMT_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

struct WhileStmt {
	struct AstNode     *condition;
	struct AstNodeList *do_stmts;
};

struct WhileStmt *
new_while_stmt ();

struct WhileStmt *
deep_copy_while_stmt (struct WhileStmt *src);

void
create_while_stmt_agedge_set (char *label,
															int len,
															struct AstNode *n);

int
interpret_while_stmt (struct AstNode *n,
											struct InterpreterCtx *ictx);

struct Operand *
compile_while_stmt (struct AstNode *n,
										struct CompilerCtx *cctx);

void
free_while_stmt (struct AstNode **n);

#endif//_WHILE_STMT_H_
