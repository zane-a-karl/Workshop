#ifndef _INTERPRETER_CONTEXT_H_
#define _INTERPRETER_CONTEXT_H_

#include "../hdr/str_hash_table.h"

struct InterpreterCtx {
	struct StrHashTable *locals;
	struct StrHashTable *funcs;
	struct StrHashTable *arrdims;//Array Dimensions
};

struct InterpreterCtx *
new_interpreter_ctx ();

void
throw_interpreter_error (char *err,
												 char *reason);

void
throw_interpreter_warning (char *warn,
													 char *reason);

void
free_interpreter_ctx (struct InterpreterCtx **ictx);

#endif//_INTERPRETER_CONTEXT_H_
