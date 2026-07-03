#ifndef _AST_H_
#define _AST_H_

#include "../hdr/forward_declarations.h"

#include "../hdr/number.h"
#include "../hdr/identifier.h"
#include "../hdr/computation.h"
#include "../hdr/var_decl.h"
#include "../hdr/func_decl.h"
#include "../hdr/assignment.h"
#include "../hdr/array_access.h"
#include "../hdr/bin_op.h"
#include "../hdr/func_call.h"
#include "../hdr/if_stmt.h"
#include "../hdr/while_stmt.h"
#include "../hdr/return_stmt.h"

#include "../hdr/interpreter_ctx.h"
#include "../hdr/operand.h"
#include "../hdr/compiler_ctx.h"

#include <graphviz/cgraph.h>

enum AstNodeType {
	IDNT,
	NUM,
	ARRACC,
  VARDECL,
	FUNCDECL,
	CMPTN,
	BINOP,
	ASSMT,
	FUNCCALL,
	IFSTMT,
	WHSTMT,
	RESTMT
};

struct AstNode {
	Agraph_t             *graph;
	Agnode_t             *self;
	int                   node_number;
	struct AstNode       *next;//ONLY MEANINGFUL IF PART OF ANL
	enum AstNodeType      type;
	union {
		struct Computation *computation;
		struct VarDecl     *var_decl;
		struct Number      *number;
		struct FuncDecl    *func_decl;
		struct Ident       *identifier;
		struct Assignment  *assignment;
		struct ArrayAccess *arr_acc;
		struct BinOp       *bin_op;
		struct FuncCall    *func_call;
		struct IfStmt      *if_stmt;
		struct WhileStmt   *while_stmt;
		struct ReturnStmt  *ret_stmt;
	};
};

struct AstNodeList {
	struct AstNode *head;
};

struct Ast {
	Agraph_t       *graph;//NOTE: this is a subgraph
	struct AstNode *root;
};

struct Ast *
new_ast (Agraph_t *top_level_graph);

struct AstNode *
new_ast_node (Agraph_t *graph,
							int       node_number,
							enum AstNodeType type);

struct AstNodeList *
new_ast_node_list ();

struct AstNode *
deep_copy_ast_node (struct AstNode *src);

struct AstNodeList *
deep_copy_ast_node_list (struct AstNodeList *src);

void
push_ast_node (struct AstNodeList *anl,
							 struct AstNode     *new_node);

void
concat_ast_list (struct AstNodeList *anl,
								 struct AstNodeList **new);

void
create_agedge_set (struct AstNode *n);

int
interpret_ast_node (struct AstNode *n,
										struct InterpreterCtx *ictx);

int
interpret_ast (struct Ast *ast);

struct Operand *
compile_ast_node (struct AstNode *n,
									struct CompilerCtx *cctx);

void
free_ast_node (struct AstNode **node);

void
free_ast_node_list (struct AstNodeList **anl);

void
free_ast (struct Ast **ast);

#endif//_AST_H_
