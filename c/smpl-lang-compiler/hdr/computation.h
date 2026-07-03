#ifndef _COMPUTATION_H_
#define _COMPUTATION_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"
#include "../hdr/operand.h"
#include "../hdr/basic_block.h"
#include "../hdr/compiler_ctx.h"

struct AstNode;

struct Computation {
	struct AstNodeList *var_decls;
	struct AstNodeList *func_decls;
	struct AstNodeList *stmts;
};

struct Computation *
new_computation ();

struct Computation *
deep_copy_computation (struct Computation *src);

void
create_computation_agedge_set (char *label,
															 int len,
															 struct AstNode *n);

int
interpret_computation (struct AstNode *n,
											 struct InterpreterCtx *ictx);

struct Operand *
compile_computation (struct AstNode *n,
										 struct CompilerCtx *cctx);

void
free_computation (struct AstNode **n);

#endif//_COMPUTATION_H_
