#include "../hdr/func_call.h"

const char *builtin_funcs[] = {"inputNum",
															 "outputNum",
															 "outputNewLine"};
const int num_builtin_funcs =
	sizeof(builtin_funcs) / sizeof(char *);

struct FuncCall *
new_func_call ()
{
	struct FuncCall *fc = calloc(1, sizeof(struct FuncCall));
	fc->ident           = NULL;
	fc->args            = new_ast_node_list();

	return fc;
}

struct FuncCall *
deep_copy_func_call (struct FuncCall *src)
{
	struct FuncCall *dst = new_func_call();
	dst->ident = deep_copy_ast_node(src->ident);
	dst->args  = deep_copy_ast_node_list(src->args);

	return dst;
}

void
create_func_call_agedge_set (char *label,
														 int len,
														 struct AstNode *n)
{
	Agedge_t *edge;
	snprintf(label, len, "Call '%s'", n->func_call->ident->identifier->name);
	agset(n->self, "label", label);
	/* create_agedge_set(n->func_call->ident); */
	agdelnode(n->graph, n->func_call->ident->self);
	for (struct AstNode *i = n->func_call->args->head; i != NULL; i = i->next) {

		edge = agedge(n->graph, n->self, i->self, NULL, TRUE);
		agset(edge, "label", "Argument");
		create_agedge_set(i);
	}
}

bool
is_builtin_function (char *name)
{
	for (int i = 0; i < num_builtin_funcs; ++i) {
		if (strncmp(name, builtin_funcs[i], MAX_FN_NAME_LEN)==0) {
			return  true;
		}
	}
	return false;
}

int
interpret_builtin_function (char                  *name,
														int                   *args,
														struct InterpreterCtx *ictx)
{
	if ( strncmp(name, "inputNum", MAX_FN_NAME_LEN) == 0 ) {

		int input = 0;
		printf("Please input a number: ");
		scanf("%d", &input);
		printf("\n");
		return input;
	} else if (strncmp(name, "outputNum", MAX_FN_NAME_LEN)==0) {
		printf("%d\n", args[0]);
	} else if ( strncmp(name,
											"outputNewLine",
											MAX_FN_NAME_LEN) == 0 ) {
		printf("\n");
	} else {
		throw_interpreter_error("Unknown builtin func: ", name);
	}
	return 0x7FFFFFFF;
}

int
interpret_func_call (struct AstNode *n,
										 struct InterpreterCtx *ictx)
{
	int rv = 0x7FFFFFFF;
	//Get the function name
	char *fn_name = n->func_call->ident->identifier->name;
	//Get the number of arguments
	int args_len = 0, k = 0;
	struct AstNode *i, *j;
	for (i = n->func_call->args->head; i != NULL; i = i->next) {
			args_len++;
	}

	//Interpret and store the args (ALL ARE `INT`s!)
	int *args = calloc(args_len, sizeof(int));
	for (i = n->func_call->args->head; i != NULL; i = i->next) {
		args[k++] = interpret_ast_node(i, ictx);
	}

	//Check if `fn_name` refers to a built-in function
	if ( is_builtin_function(fn_name) ) {
		return interpret_builtin_function(fn_name, args, ictx);
	}

	//Check if function was defined
	struct StrHashEntry *tmp = sht_lookup(ictx->funcs, fn_name);
	if ( tmp == NULL ) {
		throw_interpreter_error("Call to undefined func: ",
														fn_name);
	}
	//`ictx->funcs` maps to AstNodes holding func_decls?
	struct AstNode *fn_node = tmp->node;

	//Set up interpreter context for function body
	struct InterpreterCtx *fnctx = new_interpreter_ctx();
	fnctx->funcs = deep_copy_sht(ictx->funcs);
	for (i = fn_node->func_decl->body->local_vars->head; i != NULL; i = i->next) {
		rv = interpret_ast_node(i, fnctx);
	}

	//Eval the params and set them as fnctx locals
	char *param_name;
	for (i = fn_node->func_decl->param_idents->head,
				 j = n->func_call->args->head, k = 0;
			 i != NULL && j != NULL && k < args_len;
			 i = i->next, j = j->next, ++k) {

		param_name = i->identifier->name;
		tmp = sht_lookup(ictx->locals, param_name);
		if ( tmp != NULL ) {
			throw_interpreter_error("Var is both param and local ",
															param_name);
		}
		//Treat it like a var decl, and give it 1 int of space
		tmp = new_str_hash_entry(deep_copy_str(param_name), DATA);
		tmp->data = calloc(1, sizeof(int));
		tmp->data[0] = args[k];
		sht_insert(fnctx->locals, tmp);
	}

	//Interpret the func_body stmts and return early if required
	for (i = fn_node->func_decl->body->stmts->head; i != NULL; i = i->next) {
		rv = interpret_ast_node(i, fnctx);
		if (rv != 0x7FFFFFFF ) {
			break;
		}
	}
	free_interpreter_ctx(&fnctx);

	return rv;
}

struct Operand *
compile_func_call (struct AstNode *n,
									 struct CompilerCtx *cctx)
{
	struct OperandList *arg_ops = new_operand_list();
	struct Operand *fn_op, *rv_op;
	struct AstNode *i = n->func_call->args->head;
	int n_fn_args = 0;
	for (; i != NULL; i = i->next) {
		push_operand(arg_ops, compile_ast_node(i, cctx));
		++n_fn_args;
	}
	fn_op = new_operand(FN, n->func_call->ident->identifier->name);
	rv_op = compiler_ctx_emit(cctx, false, false, 4,
														"call", n_fn_args, fn_op, arg_ops);

	return rv_op;
}

void
free_func_call (struct AstNode **n)
{
	free_ast_node(&((*n)->func_call->ident));
	free_ast_node_list(&((*n)->func_call->args));
	free((*n)->func_call);
}
