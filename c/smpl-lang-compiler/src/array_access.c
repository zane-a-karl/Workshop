#include "../hdr/array_access.h"

struct ArrayAccess *
new_array_access ()
{
	struct ArrayAccess *aa;
	aa          = calloc(1, sizeof(struct ArrayAccess));
	aa->ident   = NULL;
	aa->indices = new_ast_node_list();

	return aa;
}

struct ArrayAccess *
deep_copy_array_access (struct ArrayAccess *src)
{
	struct ArrayAccess *dst = new_array_access();
	dst->ident = deep_copy_ast_node(src->ident);
	dst->indices = deep_copy_ast_node_list(src->indices);

	return dst;
}

void
create_array_access_agedge_set (char *label,
																int len,
																struct AstNode *n)
{
	if ( n->arr_acc->indices->head != NULL ) {
		snprintf(label, len, "Array Access '%s'", n->arr_acc->ident->identifier->name);
	} else {
		snprintf(label, len, "Access '%s'", n->arr_acc->ident->identifier->name);
	}
	agdelnode(n->graph, n->arr_acc->ident->self);
	/* create_agedge_set(n->arr_acc->ident); */
	char *tmp = calloc(len, sizeof(char));
	Agedge_t *edge;
	int idx_num = 0;
	for (struct AstNode *i = n->arr_acc->indices->head; i != NULL; i = i->next) {

		snprintf(tmp, len, "%s", "[-]");
		strlcat(label, tmp, strlen(label) + strlen(tmp) + 1);
		edge = agedge(n->graph, n->self, i->self, tmp, TRUE);
		snprintf(tmp, len, "Idx#%d", idx_num++);
		agset(edge, "label", "");
		create_agedge_set(i);
	}
	agset(n->self, "label", label);
	free(tmp);
}

int
interpret_array_access (struct AstNode *n,
												struct InterpreterCtx *ictx)
{
	int rv = 0x7FFFFFFF, final_idx = 0;
	int idx, j, k;
	struct AstNode *i;
	char *name = n->arr_acc->ident->identifier->name;
	int arrdims_len = 0;	
	struct StrHashEntry *arrdims = sht_lookup(ictx->arrdims, name);
	struct StrHashEntry *arr = sht_lookup(ictx->locals, name);
	//Check if it exists
	if ( arr == NULL ) {
		throw_interpreter_error("Access to undeclared array: ",
														name);
	}
	for (i = n->arr_acc->indices->head; i != NULL; i = i->next) {
		arrdims_len++;
	}
	k = 1;
	//Loop through indices e.g. [a][b][c] and compute
	// a*dim[k..len] + b*dim[k..len] + c*dim[k..len]
	for (i = n->arr_acc->indices->head; i != NULL; i = i->next) {

		idx = interpret_ast_node(i, ictx);
		for (j = k; j < arrdims_len; ++j) {
			idx *= arrdims->data[j];
		}
		final_idx += idx;
		k++;
	}
	rv = arr->data[final_idx];
	if ( rv == 0x7FFFFFFF ) {
		throw_interpreter_warning("Access to uninit'd array idx ",
															name);
		arr->data[final_idx] = 0;
		rv = arr->data[final_idx];
	}
	return rv;
}

struct Operand *
compile_array_access (struct AstNode *n,
											struct CompilerCtx *cctx)
{
	struct Operand *addr_op = compile_addr(n, cctx);
	struct Operand *load_op;
	load_op = compiler_ctx_emit(cctx, true, true, 2, "load", addr_op);

	return load_op;
}

struct Operand *
compile_addr (struct AstNode *n,
							struct CompilerCtx *cctx)
{
	char *name = n->arr_acc->ident->identifier->name;
	struct OpBox    base_addr_box;
	struct Operand *addr_op;
	struct Operand *base_addr_op;
	int *dims;
	
	base_addr_box = get_local(cctx->cur_block, name);
	// Need a deep copy or else you'll mix op lists :(
	base_addr_op  = deep_copy_operand(base_addr_box.op);
	dims          = base_addr_box.dims;
	struct Operand *offset_op = new_operand(IMMEDIATE, 0);
	
	struct AstNode *i = n->arr_acc->indices->head;
	int dims_len = 0;
	for (; i != NULL; i = i->next) {
		dims_len++;
	}
	i = n->arr_acc->indices->head;

	int j = 0;
	struct Operand *idx_op   , *this_offset_op;
	struct Operand *stride_op, *sizeof_int_op;
	for (; i != NULL && j < dims_len; i = i->next, ++j) {
		
		idx_op     = compile_ast_node(i, cctx);
		stride_op  = new_operand(IMMEDIATE, 1);
		for (int k = j+1; k < dims_len; ++k) {
			stride_op->number *= dims[k];
		}
		this_offset_op = compiler_ctx_emit(cctx, true, true, 3,
																			 "mul",
																			 idx_op, stride_op);
		offset_op      = compiler_ctx_emit(cctx, true, true, 3,
																			 "add",
																			 offset_op, this_offset_op);
	}
	sizeof_int_op    = new_operand(IMMEDIATE, 4);//4bytes to an int
	offset_op        = compiler_ctx_emit(cctx, true, true, 3,
																			 "mul",
																			 offset_op, sizeof_int_op);
	addr_op          = compiler_ctx_emit(cctx, true, false, 3,
																			 "adda",
																			 base_addr_op, offset_op);

	return addr_op;
}

void
free_arr_acc (struct AstNode **n)
{
	free_ast_node(&((*n)->arr_acc->ident));
	free_ast_node_list(&((*n)->arr_acc->indices));
	free((*n)->arr_acc);
}
