#include "../hdr/interpreter_ctx.h"

struct InterpreterCtx *
new_interpreter_ctx ()
{
	struct InterpreterCtx *ic;
	ic = calloc(1, sizeof(struct InterpreterCtx));
	ic->locals     = new_str_hash_table();
	ic->funcs      = new_str_hash_table();
	ic->arrdims    = new_str_hash_table();

	return ic;
}

void
throw_interpreter_error (char *err,
												 char *reason)
{
	int len = strlen(err) + strlen(reason) + 1;
	char *output = calloc(len,
												sizeof(char));
	snprintf(output, len, "%s", err);
	strlcat(output, reason, len);
	perror("ERROR");
	perror(output);
	free(reason);
	free(output);
	exit(1);
}

void
throw_interpreter_warning (char *warn,
													 char *reason)
{
	int len = strlen(warn) + strlen(reason) + 1;
	char *output = calloc(len,
												sizeof(char));
	snprintf(output, len, "%s", warn);
	strlcat(output, reason, len);
	perror("WARNING");
	perror(output);
	free(output);
}

void
free_interpreter_ctx (struct InterpreterCtx **ictx)
{
	free_sht(&(*ictx)->locals);
	free_sht(&(*ictx)->funcs);
	free_sht(&(*ictx)->arrdims);
	free((*ictx));
}
