#ifndef _FUNC_DECL_H_
#define _FUNC_DECL_H_

#include "../hdr/ast.h"
#include "../hdr/interpreter_ctx.h"
#include "../hdr/str_list.h"

#include <stdbool.h>
#include <stdlib.h>

// Wrapper so we can return local_vars and stmts from
// smpl_func_body
struct FuncBody {
	struct AstNodeList *local_vars;
	struct AstNodeList *stmts;
};

struct FuncDecl {
	bool is_void;
	struct AstNode     *fn_ident;
	struct AstNodeList *param_idents;
	struct FuncBody    *body;
};

struct FuncDecl *
new_fd ();

struct FuncBody *
new_func_body ();

struct FuncBody *
deep_copy_func_body (struct FuncBody *src);

struct FuncDecl *
deep_copy_func_decl (struct FuncDecl *src);

void
create_func_decl_agedge_set (char *label,
														 int len,
														 struct AstNode *n);

int
interpret_func_decl (struct AstNode *n,
										 struct InterpreterCtx *ictx);

struct Operand *
compile_func_decl (struct AstNode *n,
									 struct CompilerCtx *cctx);

void
free_func_decl (struct AstNode **n);

#endif//_FUNC_DECL_H_
