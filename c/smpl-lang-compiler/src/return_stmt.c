#include "../hdr/return_stmt.h"

extern char *ast_node_types[];

struct ReturnStmt *
new_return_stmt ()
{
	struct ReturnStmt *rs = calloc(1, sizeof(struct ReturnStmt));
	rs->ret_val           = NULL;

	return rs;
}

struct ReturnStmt *
deep_copy_return_stmt (struct ReturnStmt *src)
{
	struct ReturnStmt *dst = new_return_stmt();
	dst->ret_val = deep_copy_ast_node(src->ret_val);

	return dst;
}

void
create_return_stmt_agedge_set (char *label,
															 int len,
															 struct AstNode *n)
{
	Agedge_t *edge;
	agset(n->self, "label", ast_node_types[RESTMT]);
	//Check for empty return stmt
	if ( n->ret_stmt->ret_val != NULL ) {
		edge = agedge(n->graph, n->self, n->ret_stmt->ret_val->self, NULL, TRUE);
		agset(edge, "label", "Return Value");
	}
	create_agedge_set(n->ret_stmt->ret_val);
}

int
interpret_return_stmt (struct AstNode *n,
											 struct InterpreterCtx *ictx)
{
	return interpret_ast_node(n->ret_stmt->ret_val, ictx);
}

struct Operand *
compile_return_stmt (struct AstNode *n,
										 struct CompilerCtx *cctx)
{
	struct Operand *rv_op;
	rv_op = compile_ast_node(n->ret_stmt->ret_val, cctx);
	compiler_ctx_emit(cctx, "return", rv_op, false);

	return rv_op;
}

void
free_return_stmt (struct AstNode **n)
{
	free_ast_node(&((*n)->ret_stmt->ret_val));
	free((*n)->ret_stmt);
}
