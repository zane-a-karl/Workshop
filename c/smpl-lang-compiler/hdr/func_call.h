#ifndef _FUNC_CALL_H_
#define _FUNC_CALL_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

struct FuncCall {
	struct AstNode     *ident;
	struct AstNodeList *args;
};

struct FuncCall *
new_func_call ();

struct FuncCall *
deep_copy_func_call (struct FuncCall *src);

void
create_func_call_agedge_set (char *label,
														 int len,
														 struct AstNode *n);

bool
is_builtin_function (char *name);

int
interpret_builtin_function (char                  *name,
														int                   *args,
														struct InterpreterCtx *ictx);

int
interpret_func_call (struct AstNode *n,
										 struct InterpreterCtx *ictx);

struct Operand *
compile_func_call (struct AstNode *n,
									 struct CompilerCtx *cctx);

void
free_func_call (struct AstNode **n);

#endif//_FUNC_CALL_H_
