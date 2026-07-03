#include "../hdr/number.h"

struct Number *
new_number ()
{
	struct Number *n = calloc(1, sizeof(struct Number));
	n->val           = 0;
	return n;
}

struct Number *
deep_copy_number (struct Number *src)
{
	struct Number *dst = new_number();
	dst->val =	src->val;
	
	return dst;
}

void
create_num_agedge_set (char *label,
											 int len,
											 struct AstNode *n)
{
	snprintf(label, len, "%i", n->number->val);
	agset(n->self, "label", label);
}

int
interpret_number (struct AstNode *n,
									struct InterpreterCtx *ictx)
{
	return n->number->val;
}

struct Operand *
compile_number (struct AstNode *n,
								struct CompilerCtx *cctx)

{
	return new_operand(IMMEDIATE, n->number->val);
}

void
free_number (struct AstNode **n)
{
	free((*n)->number);
}
