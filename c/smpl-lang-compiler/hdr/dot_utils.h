#ifndef _DOT_UTILS_H_
#define _DOT_UTILS_H_

#include "../hdr/ast.h"
#include "../hdr/compiler_ctx.h"

#include <stdio.h>


void
gen_dot_graph (struct Ast *ast,
							 FILE *fout);

void
gen_ir_dot_graph (struct CompilerCtx *ir,
									FILE *fout);

#endif//_DOT_UTILS_H_
