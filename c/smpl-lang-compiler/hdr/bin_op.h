#ifndef _BIN_OP_H_
#define _BIN_OP_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"

struct BinOp {
	struct AstNode *opa;
	char           *op;
	struct AstNode *opb;
};

struct BinOp *
new_bin_op ();

struct BinOp *
deep_copy_bin_op (struct BinOp *src);

void
create_bin_op_agedge_set (char           *label,
													int             len,
													struct AstNode *n);

int
interpret_bin_op (struct AstNode        *n,
									struct InterpreterCtx *ictx);

struct Operand *
compile_bin_op (struct AstNode     *n,
								struct CompilerCtx *cctx);

struct Operand *
compile_conditional_jump (struct AstNode     *n,
													struct CompilerCtx *cctx,
													struct BasicBlock  *bb);

void
free_bin_op (struct AstNode **n);

#endif//_BIN_OP_H_
