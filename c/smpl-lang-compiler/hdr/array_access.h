#ifndef _ARRAY_ACCESS_H_
#define _ARRAY_ACCESS_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"
#include "../hdr/compiler_ctx.h"

struct ArrayAccess {
	struct AstNode     *ident;
	struct AstNodeList *indices;
};

struct ArrayAccess *
new_array_access ();

struct ArrayAccess *
deep_copy_array_access (struct ArrayAccess *src);

void
create_array_access_agedge_set (char           *label,
																int             len,
																struct AstNode *n);

int
interpret_array_access (struct AstNode        *n,
												struct InterpreterCtx *ictx);

struct Operand *
compile_array_access (struct AstNode     *n,
											struct CompilerCtx *cctx);

struct Operand *
compile_addr (struct AstNode     *n,
							struct CompilerCtx *cctx);

void
free_arr_acc (struct AstNode **n);

#endif//_ARRAY_ACCESS_H_
