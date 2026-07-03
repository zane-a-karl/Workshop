#include "../hdr/dot_utils.h"


/**
 * Objective here is to make all of the agedge() connections
 * with the necessary visual attributes that we weren't able
 * to do on the fly during parsing.
 */
void
gen_dot_graph (struct Ast *ast,
							 FILE *fout)
{
	// color init
	agattr(ast->graph, AGRAPH, "color", "black");
	agattr(ast->graph, AGNODE, "color", "black");
	agattr(ast->graph, AGEDGE, "color", "black");

	// label init
	agattr(ast->graph, AGNODE, "label", "default_node");
	agattr(ast->graph, AGEDGE, "label", "default_edge");

	// shape init
	agattr(ast->graph, AGNODE, "shape", "hexagon");
	
	create_agedge_set(ast->root);
	agwrite(ast->graph, fout);
}

/**
 * Objective here is to make the entire IR 
 * with the necessary visual attributes. None of this
 * is done on the fly during IR creation because
 * instructions can get deleted and changed and such
 * so it's not worth the hassle of making visualizations
 * that you'll have to remove.
 */
void
gen_ir_dot_graph (struct CompilerCtx *ir,
									FILE *fout)
{
	// color init
	agattr(ir->graph, AGRAPH, "color", "black");
	agattr(ir->graph, AGNODE, "color", "black");
	agattr(ir->graph, AGEDGE, "color", "black");

	// label init
	agattr(ir->graph, AGNODE, "label", "default_node");
	agattr(ir->graph, AGEDGE, "label", "default_edge");

	// style init
	agattr(ir->graph, AGEDGE, "style", "solid");

	// shape init
	agattr(ir->graph, AGNODE, "shape", "record");
	
	create_ir_node_set(ir);
	agwrite(ir->graph, fout);
}
