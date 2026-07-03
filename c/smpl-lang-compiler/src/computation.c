#include "../hdr/computation.h"

extern char *ast_node_types[];

struct Computation *
new_computation ()
{
	struct Computation *c;
	c             = calloc(1, sizeof(struct Computation));
	c->var_decls  = new_ast_node_list();
	c->func_decls = new_ast_node_list();
	c->stmts      = NULL;

	return c;
}

struct Computation *
deep_copy_computation (struct Computation *src)
{
	struct Computation *dst = new_computation();
	dst->var_decls = deep_copy_ast_node_list(src->var_decls);
	dst->func_decls = deep_copy_ast_node_list(src->func_decls);
	dst->stmts = deep_copy_ast_node_list(src->stmts);

	return dst;
}

void
create_computation_agedge_set (char *label,
															 int len,
															 struct AstNode *n)
{
	agset(n->self, "label", ast_node_types[CMPTN]);
	Agedge_t *edge;
	struct AstNode *i;
	i = n->computation->var_decls->head;
	for (; i != NULL; i = i->next) {

		snprintf(label, len, "%s", ast_node_types[VARDECL]);
		edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
		agset(edge, "label", label);
		agset(edge, "color", "blue");
		create_agedge_set(i);
	}
	memset(label, '\0', len*sizeof(char));
	i = n->computation->func_decls->head;
	for (; i != NULL; i = i->next) {

		snprintf(label, len, "%s", ast_node_types[FUNCDECL]);
		edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
		agset(edge, "label", label);
		agset(edge, "color", "yellow");
		create_agedge_set(i);
	}
	memset(label, '\0', len*sizeof(char));
	i = n->computation->stmts->head;
	for (; i != NULL; i = i->next) {

		snprintf(label, len, "%s", "Statement");
		edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
		agset(edge, "label", label);
		create_agedge_set(i);
	}
}

int
interpret_computation (struct AstNode *n,
											 struct InterpreterCtx *ictx)
{
	struct AstNode *i;
	int rv = 0;
	i = n->computation->var_decls->head;
	for (; i != NULL; i = i->next) {
		rv = interpret_ast_node(i, ictx);
	}
	i = n->computation->func_decls->head;
	for (; i != NULL; i = i->next) {
		rv = interpret_ast_node(i, ictx);
	}
	i = n->computation->stmts->head;
	for (; i != NULL; i = i->next) {

		rv = interpret_ast_node(i, ictx);
		if ( rv != 0x7FFFFFFF ) {
			break;
		}
	}
	return rv;
}

struct Operand *
compile_computation (struct AstNode *n,
										 struct CompilerCtx *cctx)
{
	struct BasicBlock *root_block = new_basic_block(cctx);
	struct BlockGroup *main_fn    = new_block_group("main");
	main_fn->is_main              = true;
	main_fn->entry                = root_block;
	root_block->function          = main_fn;
	cctx->cur_block               = root_block;
	push_basic_block(cctx->roots, root_block, ROOTS);
	struct AstNode *i;
	i = n->computation->var_decls->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}
	i = n->computation->func_decls->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}
	i = n->computation->stmts->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}
	main_fn->exit = cctx->cur_block;
	compiler_ctx_emit(cctx, false, false, 1, "end");

	return NULL;
}

void
free_computation (struct AstNode **n)
{
	free_ast_node_list(&((*n)->computation->var_decls));
	free_ast_node_list(&((*n)->computation->func_decls));
	free_ast_node_list(&((*n)->computation->stmts));
	free((*n)->computation);
}
