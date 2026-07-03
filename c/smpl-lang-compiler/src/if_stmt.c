#include "../hdr/if_stmt.h"

extern char *ast_node_types[];

struct IfStmt *
new_if_stmt ()
{
	struct IfStmt *is = calloc(1, sizeof(struct IfStmt));
	is->condition     = NULL;
	is->then_stmts    = NULL;
	is->else_stmts    = NULL;

	return is;
}

struct IfStmt *
deep_copy_if_stmt (struct IfStmt *src)
{
	struct IfStmt *dst = new_if_stmt();
	dst->condition = deep_copy_ast_node(src->condition);
	dst->then_stmts = deep_copy_ast_node_list(src->then_stmts);
	dst->else_stmts = deep_copy_ast_node_list(src->else_stmts);

	return dst;
}

void
create_if_stmt_agedge_set (char *label,
													 int len,
													 struct AstNode *n)
{
	agset(n->self, "label", ast_node_types[IFSTMT]);
	Agedge_t *edge;
	edge = agedge(n->graph, n->self, n->if_stmt->condition->self, NULL, TRUE);
	agset(edge, "label", "Condition");
	create_agedge_set(n->if_stmt->condition);
	for (struct AstNode *i = n->if_stmt->then_stmts->head; i != NULL; i = i->next) {

		edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
		agset(edge, "label", "True");
		agset(edge, "color", "green");
		create_agedge_set(i);
	}
	if ( n->if_stmt->else_stmts != NULL ) {
		for (struct AstNode *i = n->if_stmt->else_stmts->head; i != NULL; i = i->next) {

			edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
			agset(edge, "label", "False");
			agset(edge, "color", "red");
			create_agedge_set(i);
		}
	}
}

int
interpret_if_stmt (struct AstNode *n,
									 struct InterpreterCtx *ictx)
{
	int rv = 0x7FFFFFFF;
	struct AstNode *i;
	int cond = interpret_ast_node(n->if_stmt->condition, ictx);
	if ( cond ) {

		for (i=n->if_stmt->then_stmts->head; i!=NULL; i=i->next) {
			rv = interpret_ast_node(i, ictx);
		}
	} else {
		if ( n->if_stmt->else_stmts != NULL ) {
			for (i=n->if_stmt->else_stmts->head;
					 i!=NULL; i=i->next) {

				rv = interpret_ast_node(i, ictx);
			}
		}
	}
	return rv;
}

struct Operand *
compile_if_stmt (struct AstNode *n,
								 struct CompilerCtx *cctx)
{
	struct BasicBlock *then_block = new_basic_block(cctx);
	copy_block_ctx_params(then_block, cctx->cur_block);
	struct BasicBlock *else_block = new_basic_block(cctx);
	copy_block_ctx_params(else_block, cctx->cur_block);
	struct BasicBlock *join_block = new_basic_block(cctx);
	copy_block_ctx_params(join_block, cctx->cur_block);

	add_successor(cctx->cur_block, then_block);
	add_successor(cctx->cur_block, else_block);

	push_basic_block(cctx->cur_block->dominatees, then_block, DOMEES);
	push_basic_block(cctx->cur_block->dominatees, else_block, DOMEES);
	push_basic_block(cctx->cur_block->dominatees, join_block, DOMEES);

	compile_conditional_jump(n->if_stmt->condition, cctx, else_block);

	cctx->cur_block = then_block;
	struct AstNode *i = n->if_stmt->then_stmts->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}
	struct Operand *label_op = new_operand(LABEL, join_block->label);
	compiler_ctx_emit(cctx, false, false, 2, "bra", label_op);
	add_successor(cctx->cur_block, join_block);
	then_block = cctx->cur_block;

	cctx->cur_block = else_block;
	if (n->if_stmt->else_stmts != NULL ) {
		i = n->if_stmt->else_stmts->head;
		for (; i != NULL; i = i->next) {
			compile_ast_node(i, cctx);
		}
	}
	add_successor(cctx->cur_block, join_block);
	else_block = cctx->cur_block;

	cctx->cur_block = join_block;
	struct OpBox a, b;
	struct Operand *phi_op, *then_op, *else_op;
	struct StrHashEntry *j;
	for (int k = 0; k < MAX_NUM_VARS; ++k) {
		j = cctx->cur_block->locals_op->entries[k];
		for (; j != NULL; j = j->next) {
			a = get_local(then_block, j->name);
			b = get_local(else_block, j->name);
			//			if ( a.op == b.op) {
			if ( eq_operands(a.op, b.op) ) {			
				continue;
			}
			then_op = new_operand(LABEL, then_block->label);
			else_op = new_operand(LABEL, else_block->label);
			phi_op = compiler_ctx_emit(cctx, true, false, 5,
																 "phi",
																 then_op, deep_copy_operand(a.op),
																 else_op, deep_copy_operand(b.op));
			set_local_op(cctx->cur_block, j->name, phi_op);
		}
	}
	return NULL;
}

void
free_if_stmt (struct AstNode **n)
{
	free_ast_node(&((*n)->if_stmt->condition));
	free_ast_node_list(&((*n)->if_stmt->then_stmts));
	if ( (*n)->if_stmt->else_stmts != NULL ) {
		free_ast_node_list(&((*n)->if_stmt->else_stmts));
	}
	free((*n)->if_stmt);
}
