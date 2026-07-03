#include "../hdr/identifier.h"

struct Ident *
new_ident ()
{
	struct Ident *ident = calloc(1, sizeof(struct Ident));
	ident->name         = calloc(MAX_VAR_NAME_LEN,
															 sizeof(char));
	return ident;
}

struct Ident *
deep_copy_ident (struct Ident *src)
{
	struct Ident *dst = new_ident();
	for (int i = 0; i < MAX_VAR_NAME_LEN; ++i) {
		dst->name[i] =	src->name[i];
	}
	return dst;
}

void
create_ident_agedge_set (char *label,
												 int len,
												 struct AstNode *n)
{
	snprintf(label, len, "%s", n->identifier->name);
	agset(n->self, "label", label);
}

int
interpret_identifier (struct AstNode *n,
											struct InterpreterCtx *ictx)
{
	int val;
	char *name = n->identifier->name;
	struct StrHashEntry *var = sht_lookup(ictx->locals, name);
	if ( var == NULL ) {
		throw_interpreter_error("Access to undeclared var: ", name);
	}
	val = var->data[0];
	if ( val == 0x7FFFFFFF ) {
		throw_interpreter_warning("Acces to uninitialized var: ",
															name);
		val = 0;
	}
	return val;
}

struct Operand *
compile_identifier (struct AstNode *n,
										struct CompilerCtx *cctx)
{
	struct OpBox ident_box;
	ident_box = get_local(cctx->cur_block, n->identifier->name);

	return deep_copy_operand(ident_box.op);
}

void
free_identifier (struct AstNode **n)
{
	free((*n)->identifier->name);
	free((*n)->identifier);
}
