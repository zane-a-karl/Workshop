#include "../hdr/func_decl.h"

struct FuncDecl *
new_fd ()
{
	struct FuncDecl *fd = calloc(1, sizeof(struct FuncDecl));
	fd->is_void         = false;
	fd->fn_ident        = NULL;
	fd->param_idents    = NULL;
	fd->body            = NULL;
	return fd;
}

struct FuncBody *
new_func_body ()
{
	struct FuncBody *fb = calloc(1, sizeof(struct FuncBody));
	fb->local_vars      = new_ast_node_list();
	fb->stmts           = NULL;

	return fb;
}

struct FuncBody *
deep_copy_func_body (struct FuncBody *src)
{
	struct FuncBody *dst = new_func_body();
	dst->local_vars = deep_copy_ast_node_list(src->local_vars);
	dst->stmts = deep_copy_ast_node_list(src->stmts);

	return dst;
}

struct FuncDecl *
deep_copy_func_decl (struct FuncDecl *src)
{
	struct FuncDecl *dst = new_fd();
	dst->is_void = src->is_void;
	dst->fn_ident = deep_copy_ast_node(src->fn_ident);
	dst->param_idents =
		deep_copy_ast_node_list(src->param_idents);
	dst->body = deep_copy_func_body(src->body);
	
	return dst;
}

void
create_func_decl_agedge_set (char *label,
														 int len,
														 struct AstNode *n)
{
	Agedge_t *edge;
	snprintf(label, len, "Function '%s'", n->func_decl->fn_ident->identifier->name);
	/* create_agedge_set(n->func_decl->fn_ident); */
	agdelnode(n->graph, n->func_decl->fn_ident->self);
	agset(n->self, "label", label);
	for (struct AstNode *i = n->func_decl->param_idents->head; i != NULL; i = i->next) {

		edge = agedge(n->graph, n->self, i->self, "", TRUE);
		agset(edge, "label", "Param");
		create_agedge_set(i);
	}
	for (struct AstNode *i = n->func_decl->body->local_vars->head; i != NULL; i = i->next) {

		edge = agedge(n->graph, n->self, i->self, "", TRUE);
		agset(edge, "label", "Local Var");
		create_agedge_set(i);
	}
	for (struct AstNode *i = n->func_decl->body->stmts->head; i != NULL; i = i->next) {

		edge = agedge(n->graph, n->self, i->self, "", TRUE);
		agset(edge, "label", "Body");
		create_agedge_set(i);
	}
}

int
interpret_func_decl (struct AstNode *n,
										 struct InterpreterCtx *ictx)
{
	//'name' freed during interpreter_ctx free
	char *name =
		deep_copy_str(n->func_decl->fn_ident->identifier->name);
	//Check if it's a new function
	if ( sht_lookup(ictx->locals, name) != NULL ) {
		throw_interpreter_error("Re-declaration of func: ", name);
	}
	//If new, add it to hash map
	struct StrHashEntry *fn = new_str_hash_entry(name, NODE);
	fn->node = n;
	sht_insert(ictx->funcs, fn);

	return 0x7FFFFFFF;//a.k.a. NULL
}

struct Operand *
compile_func_decl (struct AstNode *n,
									 struct CompilerCtx *cctx)
{
	struct BasicBlock *old_root      = cctx->cur_block;
	struct BasicBlock *fn_root_block = new_basic_block(cctx);
	struct BlockGroup *fn;
	char *name  = n->func_decl->fn_ident->identifier->name;
	fn          = new_block_group(name);
	fn->is_main = false;
	fn->entry   = fn_root_block;
	struct AstNode *i = n->func_decl->param_idents->head;
	for (; i != NULL; i = i->next) {
		push_str_node(fn->arg_names, new_str_node(i->identifier->name));
	}
	fn_root_block->function = fn;
	cctx->cur_block         = fn_root_block;
	push_basic_block(cctx->roots, fn_root_block, ROOTS);

	//Compile the fn arguments
	i = n->func_decl->param_idents->head;
	for (; i != NULL; i = i->next) {
		
		declare_local(cctx->cur_block, i->identifier->name, NULL, 0, 0);
		set_local_op(cctx->cur_block, i->identifier->name,
								 new_operand(ARGUMENT, i->identifier->name));
	}

	i = n->func_decl->body->local_vars->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}
	
	i = n->func_decl->body->stmts->head;
	for (; i != NULL; i = i->next) {
		compile_ast_node(i, cctx);
	}

	//Compiling stmts resutls in a single join block at the end
	fn->exit = cctx->cur_block;
	cctx->cur_block = old_root;

	return NULL;
}

void
free_func_decl (struct AstNode **n)
{
	free_ast_node(&((*n)->func_decl->fn_ident));
	free_ast_node_list(&((*n)->func_decl->param_idents));
	free_ast_node_list(&((*n)->func_decl->body->local_vars));
	free_ast_node_list(&((*n)->func_decl->body->stmts));
	free((*n)->func_decl->body);
	free((*n)->func_decl);
}
