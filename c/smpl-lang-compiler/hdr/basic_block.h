#ifndef _BASIC_BLOCK_H_
#define _BASIC_BLOCK_H_

#include "../hdr/forward_declarations.h"

#include "../hdr/ast.h"
#include "../hdr/str_hash_table.h"
#include "../hdr/str_list.h"
#include "../hdr/instruction.h"
#include "../hdr/operand.h"
#include "../hdr/constants.h"
#include "../hdr/compiler_ctx.h"

#include <graphviz/cgraph.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

enum BasicBlockListType {
	ROOTS,
	SUCCS,
	PREDS,
	DOMEES
};

struct GraphNode {
	Agnode_t *agn;
	char *label;
};

struct BasicBlock {
	struct InstructionList *instrs;
	char                   *label;
	struct BasicBlockList  *successors;
	struct BasicBlock      *next_s;
	struct BasicBlockList  *predecessors;
	struct BasicBlock      *next_p;
	struct StrHashTable    *locals_op;
	struct StrHashTable    *locals_dim;
	struct BlockGroup      *function;
	struct BasicBlockList  *dominatees;//BB's dom'd by this BB
	struct BasicBlock      *next_d;
	struct StrHashTable    *dom_instr_tree;
	struct BasicBlock      *next_r;//Points to next root BB
	bool                    shallow_copy;
	struct GraphNode       *node;
};

struct BasicBlockList {
	struct BasicBlock *head;
};

//A.k.a. a function, with arguments
struct BlockGroup {
	struct BasicBlock *entry;
	struct BasicBlock *exit;
	struct StrList    *arg_names;
	char              *name;
	bool               is_main;
};

struct GraphNode *
new_graph_node ();

struct BasicBlock *
new_basic_block (struct CompilerCtx *cctx);

struct BasicBlockList *
new_basic_block_list ();

struct BlockGroup *
new_block_group (char *name);

struct BlockGroup *
deep_copy_block_group (struct BlockGroup *src);

void
push_basic_block (struct BasicBlockList  *bbl,
									struct BasicBlock      *new_bb,
									enum BasicBlockListType type);

void
push_to_roots (struct BasicBlockList *bbl,
							 struct BasicBlock     *new_bb);

void
push_to_successors (struct BasicBlockList *bbl,
										struct BasicBlock     *new_bb);

void
push_to_predecessors (struct BasicBlockList *bbl,
											struct BasicBlock     *new_bb);

void
push_to_dominatees (struct BasicBlockList *bbl,
										struct BasicBlock     *new_bb);

struct Operand *
vbasic_block_emit (struct BasicBlock *bb,
									 int                instr_num,
									 char              *instr_name,
									 bool               produces_output,
									 bool               may_eliminate,
									 int                n_args,
									 va_list            args);

void
add_successor (struct BasicBlock *bb,
							 struct BasicBlock *successor_bb);

void
declare_local (struct BasicBlock *bb,
							 char              *name,
							 int               *dims,
							 int                dims_len,
							 int                alloc_size);

struct OpBox
get_local (struct BasicBlock *bb,
					 char              *name);

void
set_local_op (struct BasicBlock *bb,
							char              *name,
							struct Operand    *val);

void
rename_op (struct BasicBlock   *bb,
					 struct Operand      *old_op,
					 struct Operand      *new_op,
					 struct StrHashTable *visited);

void
copy_block_ctx_params (struct BasicBlock *dst,
											 struct BasicBlock *src);

void
add_instrs_to_label (struct BasicBlock *bb);

void
add_dominatee_nodes (struct BasicBlock *bb,
										 struct CompilerCtx *ir);

void
draw_root_graph (struct BasicBlock *root,
								 struct CompilerCtx *ir);

void
free_block_group (struct BlockGroup **bg);

void
free_basic_block (struct BasicBlock **bb);

void
free_successors_basic_block_list (struct BasicBlockList **successors);

void
free_dominatees_basic_block_list (struct BasicBlockList **dominatees);

void
free_roots_basic_block_list (struct BasicBlockList **roots);

#endif//_BASIC_BLOCK_H_
