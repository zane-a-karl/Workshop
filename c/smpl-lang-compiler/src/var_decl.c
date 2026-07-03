#include "../hdr/var_decl.h"

struct VarDecl *
new_vd ()
{
	struct VarDecl *vd = calloc(1, sizeof(struct VarDecl));
	vd->dims           = NULL;
	vd->ident          = NULL;

	return vd;
}

struct VarDecl *
deep_copy_var_decl (struct VarDecl *src)
{
	struct VarDecl *dst = new_vd();
	dst->ident          = deep_copy_ast_node(src->ident);
	dst->dims           = deep_copy_ast_node_list(src->dims);

	return dst;
}

void
create_var_decl_agedge_set (char *label,
														int len,
														struct AstNode *n)
{
	char *tmp = calloc(len, sizeof(char));
	snprintf(label, len, "Declare '%s'", n->var_decl->ident->identifier->name);
	/* create_agedge_set(n->var_decl->ident); */
	agdelnode(n->graph, n->var_decl->ident->self);
	for (struct AstNode *i = n->var_decl->dims->head; i != NULL; i = i->next) {

		snprintf(tmp, len, "[%i]", i->number->val);
		strlcat(label, tmp, strlen(label) + strlen(tmp) + 1);
		/* create_agedge_set(i); */
		agdelnode(n->graph, i->self);
	}
	free(tmp);
	agset(n->self, "label", label);
}

int
interpret_var_decl (struct AstNode *n,
										struct InterpreterCtx *ictx)
{
	int data_len, dim, arrdims_len = 0, j = 0;
	struct AstNode *i;
	//'name' freed during interpreter_ctx free
	char *name = deep_copy_str(n->var_decl->ident->identifier->name);
	//Check if it's a new variable
	if ( sht_lookup(ictx->locals, name) != NULL ) {
		throw_interpreter_error("Re-declaration of var: ", name);
	}
	//If new, add it to hash map
	struct StrHashEntry *var_ent = new_str_hash_entry(name, DATA);
	sht_insert(ictx->locals, var_ent);
	//Give it a base of 1 int of space
	data_len = 1;
	var_ent->data = calloc(data_len, sizeof(int));
	var_ent->data_len = data_len;
	//If it's an arr give it 1 int of space to save the arr dims
	struct StrHashEntry *arrdims_ent;
	if ( n->var_decl->dims->head != NULL ) {
		arrdims_ent = new_str_hash_entry(deep_copy_str(name), DATA);
		sht_insert(ictx->arrdims, arrdims_ent);
		arrdims_len = 1;
		arrdims_ent->data = calloc(arrdims_len, sizeof(int));
	}
	//If it's an arr give it the requisite space
	for (i = n->var_decl->dims->head; i != NULL; i = i->next) {

		dim = interpret_ast_node(i, ictx);
		data_len *= dim;
		var_ent->data = realloc(var_ent->data, data_len * sizeof(int));
		var_ent->data_len = data_len;
		if ( i->next != NULL ) {
			arrdims_ent->data = realloc(arrdims_ent->data,
																	(++arrdims_len) * sizeof(int));
		}
		arrdims_ent->data[j++] = dim;
	}
	//Initialize the var_ent->data to 0x7FFFFFFF=NULL
	for (j = 0; j < data_len; ++j) {
		var_ent->data[j] = 0x7FFFFFFF;//a.k.a. NULL
	}
	//Use MAX of 'int' for NULL
	return 0x7FFFFFFF;
}

struct Operand *
compile_var_decl (struct AstNode *n,
									struct CompilerCtx *cctx)
{
	char *name = n->var_decl->ident->identifier->name;
	int dims_len = 0;
	int alloc_size = 4;//4 bytes to an int

	//Get the number of dimsensions
	struct AstNode *i = n->var_decl->dims->head;
	for (; i != NULL; i = i->next) {
		dims_len++;
	}
	int *dims = NULL;
	if ( dims_len != 0) {
		dims = calloc(dims_len, sizeof(int));
	}

	//Store the values of the dimensions
	i = n->var_decl->dims->head;
	for (int j = 0; i != NULL && j < dims_len; i = i->next, ++j) {
		dims[j] = i->number->val;
		alloc_size *= dims[j];
	}

	//Create a BB instruction to declare the var
	declare_local(cctx->cur_block, name, dims, dims_len, alloc_size);

	//Create a BB 'alloca' instruction to give the arr more space
	struct Operand *base_addr;
	if ( dims_len != 0 ) {
		base_addr =
			compiler_ctx_emit(cctx, true, false, 2,
												"alloca", new_operand(IMMEDIATE,
																							alloc_size));
		set_local_op(cctx->cur_block, name, base_addr);
	}
	return NULL;
}

void
free_var_decl (struct AstNode **n)
{
	free_ast_node(&((*n)->var_decl->ident));
	free_ast_node_list(&((*n)->var_decl->dims));
	free((*n)->var_decl);
}
