#include "../hdr/utils.h"
#include "../hdr/token.h"
#include "../hdr/lexer.h"
#include "../hdr/parser.h"
#include "../hdr/dot_utils.h"

#include <assert.h>
#include <graphviz/cgraph.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc,
			char *argv[])
{
	assert(argc == 2);

	// Allocates heap memory for parser
	struct Parser *psr = new_parser(argv[1]);

	Agraph_t *tlg = agopen("Top Level Graph", Agdirected, NULL);
	// Allocates heap memory for ast
	struct Ast *ast = parse(psr, tlg);

	//GENERATE THE GRAPHVIZ OF THE AST
	/* FILE *fout = fopen("output/Test1.dot", "w+"); */
	/* gen_dot_graph(ast, fout); */
	/* fclose(fout); */

	//INTERPRET THE SMPL PROPGRAM AND OUTPUT THE RESULTS
	//	int rv = interpret_ast(ast);
	//	printf("result = %d\n", rv);

	//GENERATE THE IR AND
	//GENERATE THE GRAPHVIZ OF THE IR
	bool cse_bool = false;
	struct CompilerCtx *ir = compile(ast, cse_bool);
	FILE *fout = fopen("output/Test1.dot", "w+");
	gen_ir_dot_graph(ir, fout);
	fclose(fout);
	
	

	// Free heap memory
	free_parser(&psr);
	free_ast(&ast);
	free_compiler_ctx(&ir);

	return 0;
}
