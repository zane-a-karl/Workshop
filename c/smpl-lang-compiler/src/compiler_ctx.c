#include "../hdr/compiler_ctx.h"

struct CompilerCtx *
new_compiler_ctx (bool cse_bool)
{
	struct CompilerCtx *cc;
	cc            = calloc(1, sizeof(struct CompilerCtx));
	cc->instr_ctr = 1; // to be like andre for now
	cc->block_ctr = 0;
	cc->cur_block = NULL;
	cc->roots     = new_basic_block_list();
	cc->exec_cse  = cse_bool;
	cc->graph= agopen("Top Level Graph", Agdirected, NULL);

	return cc;
}

struct Operand *
compiler_ctx_emit (struct CompilerCtx *cctx,
									 bool                produces_output,
									 bool                may_eliminate,
									 int                 n_args,
									 ...)
{
	struct Operand *result_op;
	bool po = produces_output;
	bool me = may_eliminate;
	char *instr_name;
	va_list args;
	va_start(args, n_args);
	instr_name = va_arg(args, char *);//First arg is always the name
	result_op = vbasic_block_emit(cctx->cur_block, cctx->instr_ctr,
																instr_name, po, me, n_args-1,
																args);
	va_end(args);
	cctx->instr_ctr++;

	return result_op;
}

void
throw_compiler_error (char *err,
											char *reason)
{
	int len = strlen(err) + strlen(reason) + 1;
	char *output = calloc(len, sizeof(char));
	snprintf(output, len, "%s", err);
	strlcat(output, reason, len);
	perror("ERROR");
	perror(output);
	free(reason);
	free(output);
	exit(1);
}

void
throw_compiler_warning (char *warn,
												char *reason)
{
	int len = strlen(warn) + strlen(reason) + 1;
	char *output = calloc(len, sizeof(char));
	snprintf(output, len, "%s", warn);
	strlcat(output, reason, len);
	perror("WARNING");
	perror(output);
	free(output);
}

struct CompilerCtx *
compile (struct Ast *ast,
				 bool cse_bool)
{
	struct CompilerCtx *ir = new_compiler_ctx(cse_bool);
	compile_ast_node(ast->root, ir);

	return ir;
}

/* char *no_side_effect[] = {"let", "neg", "add", "sub", "mul", "div", "cmp", */
/* 													"adda", "load", "phi"}; */
/* const int num_nse = */
/* 	sizeof(no_side_effect)/sizeof(char); */

/* void */
/* eliminate_dead_code (struct BasicBlock *bb) */
/* { */
/* 	used_set = {op.i */
/* 		for block in self */
/* 		for instr in block.instrs */
/* 		for op in instr.ops */
/* 		if isinstance(op, InstructionOp)}; */
/* 	nse_flag = false; */
/* 	struct BasicBlock *i; */
/* 	for () { */
/* 		for (i, instr in enumerate(block.instrs)) { */
/* 			for (int j = 0; j < num_nse; ++j) { */
/* 				if (strcmp(instr->name, no_side_effect[j]) == 0) { */
/* 					nse_flag = true; */
/* 				} */
/* 			} */
/* 			if ( nse_flag && instr.i not in used_set) b){ */
/* 						del block.instrs[i]; */
/* 			} */
/* 		} */
/* 	} */

void
create_ir_node_set (struct CompilerCtx *ir)
{
	struct BasicBlock *i = ir->roots->head;
	for (; i != NULL; i = i->next_r) {
		draw_root_graph(i, ir);
	}
}

void
free_compiler_ctx (struct CompilerCtx **cctx)
{
	free_roots_basic_block_list(&((*cctx)->roots));
	//Don't free cur_block as it will be freed elsewhere
	agclose((*cctx)->graph);
	free(*cctx);
}
