#include "../hdr/basic_block.h"

struct GraphNode *
new_graph_node ()
{
	struct GraphNode *gn = calloc(1, sizeof(*gn));
	gn->agn = NULL;
	gn->label = calloc(4096, sizeof(char));

	return gn;
}


struct BasicBlock *
new_basic_block (struct CompilerCtx *cctx)
{
	cctx->block_ctr++;

	struct BasicBlock *bb = calloc(1, sizeof(struct BasicBlock));
	bb->instrs            = new_instruction_list();
	bb->label             = calloc(MAX_VAR_NAME_LEN, sizeof(char));
	snprintf(bb->label, MAX_VAR_NAME_LEN, "BB%i", cctx->block_ctr);
	bb->successors        = new_basic_block_list();
	bb->next_s            = NULL;
	bb->predecessors      = new_basic_block_list();
	bb->next_p            = NULL;
	bb->locals_op         = new_str_hash_table();
	bb->locals_dim        = new_str_hash_table();
	bb->function          = NULL;
	bb->dominatees        = new_basic_block_list();
	bb->next_d            = NULL;
	bb->dom_instr_tree    = new_str_hash_table();
	bb->next_r            = NULL;
	bb->shallow_copy      = false;
	bb->node              = new_graph_node();

	return bb;
}

struct BasicBlockList *
new_basic_block_list ()
{
	struct BasicBlockList *bbl;
	bbl       = calloc(1, sizeof(struct BasicBlockList));
	bbl->head = NULL;

	return bbl;
}

struct BlockGroup *
new_block_group (char *name)
{
	struct BlockGroup *bg = calloc(1, sizeof(struct BlockGroup));
	bg->entry             = NULL;
	bg->exit              = NULL;
	bg->arg_names         = new_str_list();
	bg->name              = deep_copy_str(name);
	bg->is_main           = false;

	return bg;
}

/** Semi-deep-copy because entry and exit point to the same
		data as the src's
 */
struct BlockGroup *
deep_copy_block_group (struct BlockGroup *src)
{
	struct BlockGroup *dst = new_block_group(src->name);
	dst->entry = src->entry;
	dst->exit = src->exit;
	dst->arg_names         = deep_copy_str_list(src->arg_names);
	dst->is_main           = src->is_main;

	return dst;
}

/* assume `new_bb` already calloc'd
 * We have to use a switch statement to push to the different BB
 * lists b/c there is a chance that the lists may share BB's and
 * thus a single `next` pointer will not be enough to iterate
 * through the list but a `next` pointer per list is necessary. The
 * switch statement allows us to determine to which list we push.
 */
void
push_basic_block (struct BasicBlockList  *bbl,
									struct BasicBlock      *new_bb,
									enum BasicBlockListType type)
{
	switch (type) {
	case ROOTS:
		push_to_roots(bbl, new_bb);
		break;
	case SUCCS:
		push_to_successors(bbl, new_bb);
		break;
	case PREDS:
		push_to_predecessors(bbl, new_bb);
		break;
	case DOMEES:
		push_to_dominatees(bbl, new_bb);
		break;
	default:
		perror("(push_basic_block): Unknown bb list type");
		exit(1);
		break;
	}
}

// assume `new_bb` already calloc'd
void
push_to_roots (struct BasicBlockList *bbl,
							 struct BasicBlock     *new_bb)
{
	struct BasicBlock *bb = bbl->head;
	if ( bb == NULL ) {
		bbl->head = new_bb;
	} else {
		while ( bb->next_r != NULL ) {
			bb = bb->next_r;
		}
		bb->next_r = new_bb;
	}
}

// assume `new_bb` already calloc'd
void
push_to_successors (struct BasicBlockList *bbl,
										struct BasicBlock     *new_bb)
{
	struct BasicBlock *bb = bbl->head;
	if ( bb == NULL ) {
		bbl->head = new_bb;
	} else {
		while ( bb->next_s != NULL ) {
			bb = bb->next_s;
		}
		bb->next_s = new_bb;
	}
}

// assume `new_bb` already calloc'd
void
push_to_predecessors (struct BasicBlockList *bbl,
											struct BasicBlock     *new_bb)
{
	struct BasicBlock *bb = bbl->head;
	if ( bb == NULL ) {
		bbl->head = new_bb;
	} else {
		while ( bb->next_p != NULL ) {
			bb = bb->next_p;
		}
		bb->next_p = new_bb;
	}
}

// assume `new_bb` already calloc'd
void
push_to_dominatees (struct BasicBlockList *bbl,
										struct BasicBlock     *new_bb)
{
	struct BasicBlock *bb = bbl->head;
	if ( bb == NULL ) {
		bbl->head = new_bb;
	} else {
		while ( bb->next_d != NULL ) {
			bb = bb->next_d;
		}
		bb->next_d = new_bb;
	}
}

/**
 *Takes the compiler_ctx_emit or basic_block_emit fns' `va_list`
 */
struct Operand *
vbasic_block_emit (struct BasicBlock *bb,
									 int                instr_num,
									 char              *instr_name,
									 bool               produces_output,
									 bool               exec_cse,
									 int                n_args,
									 va_list            args)
{
	struct Instruction *instr;
	instr = new_instruction(instr_name, produces_output, n_args, args);

	char *dom_class_name = dominance_class(instr);
	struct StrHashEntry *dom_class_entry;
	dom_class_entry = sht_lookup(bb->dom_instr_tree, dom_class_name);
	if ( dom_class_entry != NULL ) {
		instr->dominator = dom_class_entry->instruction;
	}

	//Find bb's instrs length and bb's latest_instr
	int instrs_len = 0;
	struct Instruction *latest_i;
	latest_i = bb->instrs->head;
	if ( latest_i != NULL ) {
		for (; latest_i->next != NULL; latest_i = latest_i->next) {
			++instrs_len;
		}
		++instrs_len;//b/c you miss the latest_i by stopping early.
	}

	struct Instruction *identical;
	if ( exec_cse == true ) {

		identical = find_dominating_identical(instr);// no need to calloc
		if ( identical != NULL ) {

			return new_operand(INSTRUCTION, identical->number);
		} else if ( strncmp(instr->name, "load", 5) == 0 &&
								instrs_len > 0 &&
								strncmp(latest_i->name, "adda", 5) == 0 &&
								instr->ops->head->type == INSTRUCTION &&
								instr->ops->head->number == latest_i->number
								) {

			struct Instruction *identical_adda;
			identical_adda = find_dominating_identical(latest_i);
			if ( identical_adda != NULL ) {

				/* struct OperandList *orig_ops = instr->ops; */
				/* instr->ops = new_operand_list(); */
				/* push_operand(instr->ops, */
				/* 						 new_operand(INSTRUCTION, */
				/* 												 identical_adda->number)); */
				int orig_op_num = instr->ops->head->number;
				instr->ops->head->number = identical_adda->number;
				identical = find_dominating_identical(instr);
				if ( identical != NULL ) {
					//					sht_delete(bb->dom_instr_tree, identical->name);
					delete_instruction(bb->instrs, latest_i);
					
					struct StrHashEntry *ent = sht_lookup(bb->dom_instr_tree, dominance_class(latest_i));
					//					delete_instruction_from_sht(&ent, latest_i);
					delete_instruction_from_sht(ent, latest_i);
					//					latest_i->dominator = identical_adda
					return new_operand(INSTRUCTION, identical->number);
				} else {

					//					free_operand_list(&(instr->ops));
					//					instr->ops = orig_ops;
				instr->ops->head->number = orig_op_num;
				}//identical!=NULL
			}//identical_adda!=NULL
		}//long else if
	}//exec_cse
	instr->number = instr_num;
	push_instruction(bb->instrs, instr);
	if (dom_class_entry == NULL) {
		dom_class_entry =
			new_str_hash_entry(deep_copy_str(dom_class_name), INST);
		sht_insert(bb->dom_instr_tree, dom_class_entry);
	}
	dom_class_entry->instruction = instr;//Shallow copy DON'T FREE!

	return new_operand(INSTRUCTION, instr->number);
}

void
add_successor (struct BasicBlock *bb,
							 struct BasicBlock *successor_bb) {

	push_basic_block(bb->successors, successor_bb, SUCCS);
	push_basic_block(successor_bb->predecessors, bb, PREDS);
}

/**
 * Declare a local var form this block and set it to an uninitialied
 * value.
 * Assume `dims` has already been calloc'd but NOT `name`
 */
void
declare_local (struct BasicBlock *bb,
							 char              *name,
							 int               *dims,
							 int                dims_len,
							 int                alloc_size)
{
	struct StrHashEntry *var_ent  = sht_lookup(bb->locals_op , name);
	struct StrHashEntry *dims_ent = sht_lookup(bb->locals_dim, name);
	if ( var_ent != NULL || dims_ent != NULL ) {
		throw_compiler_error("Redclaration of local var: ", name);
	}

	var_ent           = new_str_hash_entry(deep_copy_str(name), OP);
	var_ent->operand  = NULL;
	var_ent->data_len = alloc_size;
	sht_insert(bb->locals_op, var_ent);

	dims_ent = new_str_hash_entry(deep_copy_str(name), DATA);
	dims_ent->data_len = dims_len;
	dims_ent->data     = dims;
	sht_insert(bb->locals_dim, dims_ent);
	//andre doesn't insert data but just inserts 'None' should you?
}

struct OpBox
get_local (struct BasicBlock *bb,
					 char              *name)
{
	struct StrHashEntry *var = sht_lookup(bb->locals_op, name);
	if ( var == NULL ) {
		throw_compiler_error("Access to undeclared local: ", name);
	}
	struct Operand *var_op    = var->operand;
	struct StrHashEntry *dims = sht_lookup(bb->locals_dim, name);
	if ( var_op == NULL ) {
		return new_op_box(new_operand(UNINITVAR, name), dims->data);
	}
	return new_op_box(var_op, dims->data);
}

/**
 *Assume `op` is already calloc'd
 */
void
set_local_op (struct BasicBlock *bb,
							char              *name,
							struct Operand    *op)
{
	struct StrHashEntry *local_entry = sht_lookup(bb->locals_op, name);
	if ( local_entry == NULL ) {
		throw_compiler_error("Access to undeclared local: ", name);
	}
	local_entry->type = OP;
	local_entry->operand = op;
	// Andre inserts the op data here should you too? I don't think so
}

/** Both `old` and `new` are already calloc'd
 */
void
rename_op (struct BasicBlock   *bb,
					 struct Operand      *old_op,
					 struct Operand      *new_op,
					 struct StrHashTable *visited)
{
	if ( visited == NULL ) {
		visited = new_str_hash_table();
	}
	if ( sht_lookup(visited, bb->label) != NULL ) {
		free_sht(&visited); // Doesn't free bb
		return;
	}

	struct StrHashEntry *bb_entry;
	bb_entry = new_str_hash_entry(bb->label, BB);
	sht_insert(visited, bb_entry);
	bb_entry->basic_block = bb;

	struct Instruction *i;
	struct Operand *j;
	for (i = bb->instrs->head; i != NULL; i = i->next) {
		for (j = i->ops->head; j != NULL; j = j->next) {
			if ( eq_operands(j, old_op) ) {
				j->type = new_op->type;
				switch ( new_op->type ) {
				case IMMEDIATE:
				case INSTRUCTION:
					j->number = new_op->number;
					break;
				case UNINITVAR:
				case ARGUMENT:
				case LABEL:
				case FN:
					j->name   = new_op->name;
					break;
				case POSSPHI:
					j->op     = new_op->op;
					break;
				default:
					perror("(rename_op): Unknown operand type");
					exit(1);
					break;
				}
			}//if
		}//jfor
	}//ifor

	struct BasicBlock *succ = bb->successors->head;
	for (; succ != NULL; succ = succ->next_s) {
		rename_op(succ, old_op, new_op, visited);
	}
}

void
copy_block_ctx_params (struct BasicBlock *dst,
											 struct BasicBlock *src)
{
	// This is a deep copy of the sht struct but a shallow
	// copy of the data within. I.e. dst will have its own
	// mem for its own tables, but the content of the tables
	// will be the same pointers as src's.
	dst->locals_op      = deep_copy_sht(src->locals_op);
	dst->locals_dim     = deep_copy_sht(src->locals_dim);
	dst->function       = deep_copy_block_group(src->function);
	//	dst->dom_instr_tree = deep_copy_sht(src->dom_instr_tree);
	// I think the dom_instr_tree should be shared not deep copied!
	dst->dom_instr_tree = src->dom_instr_tree;
	dst->shallow_copy = true;
	
}

void
add_instrs_to_label (struct BasicBlock *bb)
{
	char *label = bb->node->label;
	if ( bb->instrs != NULL ) {
		struct Instruction *i = bb->instrs->head;
		for (; i != NULL; i = i->next) {
			char *instr = create_label_from_instr(i);
			strlcat(label, instr,
							strlen(label) +
							strlen(instr) + 1);
			if ( i->next != NULL ) {
				strlcat(label, " |", strlen(label) + 2 + 1);
			}
		}
		strlcat(label, " }", strlen(label) + 2 + 1);
	} 
	agset(bb->node->agn, "label", bb->node->label);
}

void
add_dominatee_nodes (struct BasicBlock *bb,
										 struct CompilerCtx *ir)
{
	if ( bb->dominatees != NULL ) {
		struct BasicBlock *d = bb->dominatees->head;
		Agedge_t *edge;
		Agraph_t *graph = ir->graph;
		Agnode_t *self = bb->node->agn;
		Agnode_t *d_self;
		for (; d != NULL; d = d->next_d) {
			d->node->agn = agnode(graph, d->label, TRUE);
			d_self = d->node->agn;
			edge = agedge(graph, self, d_self, NULL, TRUE);
			agset(edge, "label", "dom");
			agset(edge, "color", "blue");
			agset(edge, "style", "dotted");
			//			create_dominatee_nodes(d);
			strlcpy(d->node->label, d->label,
							strnlen(d->node->label, 4096) +
							strnlen(d->label, MAX_VAR_NAME_LEN) + 1);
			strlcat(d->node->label, " | { ",
							strnlen(d->node->label, 4096) +
							6 + 1);
			agset(d->node->agn, "label", d->node->label);

			add_instrs_to_label(d);

		}
		//		agset(bb->node->agn, "label", bb->node->label);
	}
}

void
draw_root_graph (struct BasicBlock *root,
								 struct CompilerCtx *ir)
{
	Agraph_t *graph = ir->graph;
	root->node->agn = agnode(graph, root->label, TRUE);
	strlcpy(root->node->label, root->label,
					strnlen(root->node->label, 4096) +
					strnlen(root->label, MAX_VAR_NAME_LEN) + 1);
	strlcat(root->node->label, " | { ",
					strnlen(root->node->label, 4096) +
					6 + 1);
	agset(root->node->agn, "label", root->node->label);

	add_instrs_to_label(root);

	add_dominatee_nodes(root, ir);
}

void
free_block_group (struct BlockGroup **bg)
{
	//Don't free `entry` it was freed elsewhere
	//Don't free `exit` it was freed elsewhere
	free_str_list(&(*bg)->arg_names);
	free((*bg)->name);
	free(*bg);
}

void
free_basic_block (struct BasicBlock **bb)
{
	free_instruction_list(&(*bb)->instrs);
	//Don't free `lastest_instr` it was freed in line above.
	free((*bb)->label);
	(*bb)->label = NULL;

	free((*bb)->successors);//Just the pointer not the whole list
	(*bb)->successors = NULL;
	//Don't free `next_s` it was never calloc'd

	free((*bb)->predecessors);//Just the pointer not the whole list
	(*bb)->predecessors = NULL;
	//Don't free `next_p` it was never calloc'd

	// If's and While's will have the same hash tables and bg!
	//	if ( !(*bb)->shallow_copy ) {
		free_sht(&((*bb)->locals_op));
		free_sht(&((*bb)->locals_dim));
		free_block_group(&((*bb)->function));
		//	}

	//Don't free `dominatees` it was freed elsewhere
	/* free((*bb)->dominatees);//Just the pointer not the whole list */
	/* (*bb)->dominatees = NULL; */
	//Don't free `next_d` it was never calloc'd

	if ( !(*bb)->shallow_copy ) {
		free_sht(&((*bb)->dom_instr_tree));
	}
	free((*bb)->node->label);
	//Don't free `next_r` it was never calloc'd
	free(*bb);
	*bb = NULL;
}

/**
 * We only free the successors list because the predecessors and
 * dominatees lists' basic blocks will be simultaneously freed as a
 * consequence of freeing the successors list.
 */
void
free_successors_basic_block_list (struct BasicBlockList **successors)
{
	struct BasicBlock *cur = (*successors)->head;
	struct BasicBlock *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur = cur->next_s;
		free_basic_block(&prv);
	}
	free(*successors);
	*successors = NULL;
}

void
free_dominatees_basic_block_list (struct BasicBlockList **dominatees)
{
	struct BasicBlock *cur = (*dominatees)->head;
	if (cur != NULL) {
		free_dominatees_basic_block_list( &(cur->dominatees) );
	}
	struct BasicBlock *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur = cur->next_d;
		free_basic_block(&prv);
	}
	free(*dominatees);
	*dominatees = NULL;
}

void
free_roots_basic_block_list (struct BasicBlockList **roots)
{
	struct BasicBlock *cur = (*roots)->head;
	struct BasicBlock *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur = cur->next_r;
		//		free_successors_basic_block_list(&(prv->successors));
		free_dominatees_basic_block_list( &(prv->dominatees) );
		free_basic_block(&prv);
	}
	free(*roots);
	*roots = NULL;
}
