#include "../hdr/token.h"


const char possible_individual_symbols[] = {
	'!', '=', '<', '>', '[', ']', '(', ')', '{',
	'}', '*', '/', '+', '-', ';', '.', ','
};
const int num_possible_individual_symbols =
	sizeof(possible_individual_symbols)/sizeof(char);

const char *possible_symbols[] = {
	"!=", "==", "<", "<=", ">",	">=",  "[", "]", "(", ")",
	"{" ,  "}", "*",  "/", "+",	 "-", "<-", ";", ".", ","
};
const int num_possible_symbols =
	sizeof(possible_symbols)/sizeof(char *);

const char *tkn_types[] = {
	"IDENT",
	"NUMBER",
	"OP_INEQ",
	"OP_EQ",
	"OP_LT",
	"OP_LE",
	"OP_GT",
	"OP_GE",
	"LBRACKET",
	"RBRACKET",
	"LPAREN",
	"RPAREN",
	"LBRACE",
	"RBRACE",
	"ASTERISK",
	"SLASH",
	"PLUS",
	"MINUS",
	"LET",
	"LARROW",
	"CALL",
	"IF",
	"THEN",
	"ELSE",
	"FI",
	"WHILE",
	"DO",
	"OD",
	"RETURN",
	"VAR",
	"ARRAY",
	"SEMICOLON",
	"VOID",
	"FUNCTION",
	"MAIN",
	"PERIOD",
	"COMMA",
};

struct Token *
new_token ()
{
	struct Token *t = calloc(1, sizeof(struct Token));
	t->type  = 0;
	t->raw   = calloc(MAX_TKN_LEN, sizeof(char));
	t->val   = 0;
	t->line  = 0;
	return t;
}

/**
 *  Note that `tn->tkn != new_token()` because a token node
 * will never exist in a vacuum. It will always be created
 * with the intention of packaging an existing and previously
 * allocated token.
 */
struct TokenNode *
new_token_node (struct Token *t)
{
	struct TokenNode *tn = calloc(1,	sizeof(struct TokenNode));
	tn->tkn              = t;
	tn->next             = NULL;

	return tn;
}

/**
 *  Note that `tl->head != new_token_node()` because pushing
 * to the head should be the same as pushing anywhere and
 * pushing then, should require mem allocation,
 * i.e. `new_token_node()`, to be called.
 */
struct TokenList *
new_token_list ()
{
	struct TokenList *tl = calloc(1, sizeof(struct TokenList));
	tl->head             = NULL;

	return tl;
}

void
next_token (struct TokenNode **tn)
{
	(*tn) = (*tn)->next;
}

struct TokenNode *
push_token (struct TokenList *tl,
						struct Token *t)
{
	struct TokenNode *new_node = new_token_node(t);
	struct TokenNode *i = tl->head;
	if ( i == NULL ) {
		tl->head = new_node;
	} else {
		while ( i->next != NULL ) {
			i = i->next;
		}
		i->next = new_node;
	}
	return new_node;
}

void
print_token_list (struct TokenList *tl)
{
	int idx = 0;
  for (struct TokenNode *i = tl->head; i != NULL; i=i->next) {
    printf("Tkn %d: type=%d, raw=\"%s\", val=%i, line=%i\n",
           idx++,
           i->tkn->type,
           i->tkn->raw,
           i->tkn->val,
           i->tkn->line);
  }
}

void
free_token (struct Token **t)
{
	free((*t)->raw);
	free(*t);
	*t = NULL;
}

void
free_token_node (struct TokenNode **tn)
{
	free_token(&((*tn)->tkn));
	free(*tn);
	*tn = NULL;
}

void
free_token_list (struct TokenList **tl)
{
	struct TokenNode *cur = (*tl)->head;
	struct TokenNode *prv;
  while ( cur != NULL ) {
		prv = cur;
		cur = cur->next;
		free_token_node(&prv);
	}
	free(*tl);
	*tl = NULL;
}

struct Token *
new_alnum_token (char *buf,
								 int line)
{
	regex_t re_ident, re_number;
	int rv_ident = regcomp(&re_ident,
												 "^[a-zA-Z][a-zA-Z0-9]*$",
												 REG_EXTENDED);
	check_regex_compilation(rv_ident);
	int rv_number = regcomp(&re_number,
													"^[0-9]+$",
													REG_EXTENDED);
	check_regex_compilation(rv_number);

	struct Token *t = new_token();
	strncpy(t->raw, buf, MAX_TKN_LEN);
	t->line = line;

	if ( strncmp(buf, "main", MAX_TKN_LEN) == 0 ) {
		t->type = MAIN;
	} else if ( strncmp(buf, "function", MAX_TKN_LEN) == 0 ) {
		t->type = FUNCTION;
	} else if ( strncmp(buf, "void", MAX_TKN_LEN) == 0 ) {
		t->type = VOID;
	} else if ( strncmp(buf, "array", MAX_TKN_LEN) == 0 ) {
		t->type = ARRAY;
	} else if ( strncmp(buf, "var", MAX_TKN_LEN) == 0 ) {
		t->type = VAR;
	} else if ( strncmp(buf, "return", MAX_TKN_LEN) == 0 ) {
		t->type = RETURN;
	} else if ( strncmp(buf, "od", MAX_TKN_LEN) == 0 ) {
		t->type = OD;
	} else if ( strncmp(buf, "do", MAX_TKN_LEN) == 0 ) {
		t->type = DO;
	} else if ( strncmp(buf, "while", MAX_TKN_LEN) == 0 ) {
		t->type = WHILE;
	} else if ( strncmp(buf, "fi", MAX_TKN_LEN) == 0 ) {
		t->type = FI;
	} else if ( strncmp(buf, "else", MAX_TKN_LEN) == 0 ) {
		t->type = ELSE;
	} else if ( strncmp(buf, "then", MAX_TKN_LEN) == 0 ) {
		t->type = THEN;
	} else if ( strncmp(buf, "if", MAX_TKN_LEN) == 0 ) {
		t->type = IF;
	} else if ( strncmp(buf, "call", MAX_TKN_LEN) == 0 ) {
		t->type = CALL;
	} else if ( strncmp(buf, "let", MAX_TKN_LEN) == 0 ) {
		t->type = LET;
	} else if ( !regexec(&re_number, buf, 0, NULL, 0) ) {
		t->type = NUMBER;
		sscanf(buf, "%i", &(t->val));
	} else if ( !regexec(&re_ident, buf, 0, NULL, 0) ) {
		t->type = IDENT;
	} else {
		perror("Unknown character sequence");
	}

	return t;
}

struct Token *
new_symbol_token (char *buf,
									int line)
{
	struct Token *t = new_token();
	strncpy(t->raw, buf, MAX_TKN_LEN);

	t->line = line;

	if ( strncmp(buf, "!=", MAX_TKN_LEN) == 0 ) {
		t->type = OP_INEQ;
	} else if ( strncmp(buf, "==", MAX_TKN_LEN) == 0 ) {
		t->type = OP_EQ;
	} else if ( strncmp(buf, "<", MAX_TKN_LEN) == 0 ) {
		t->type = OP_LT;
	} else if ( strncmp(buf, "<=", MAX_TKN_LEN) == 0 ) {
		t->type = OP_LE;
	} else if ( strncmp(buf, ">", MAX_TKN_LEN) == 0 ) {
		t->type = OP_GT;
	} else if ( strncmp(buf, ">=", MAX_TKN_LEN) == 0 ) {
		t->type = OP_GE;
	} else if ( strncmp(buf, "[", MAX_TKN_LEN) == 0 ) {
		t->type = LBRACKET;
	} else if ( strncmp(buf, "]", MAX_TKN_LEN) == 0 ) {
		t->type = RBRACKET;
	} else if ( strncmp(buf, "(", MAX_TKN_LEN) == 0 ) {
		t->type = LPAREN;
	} else if ( strncmp(buf, ")", MAX_TKN_LEN) == 0 ) {
		t->type = RPAREN;
	} else if ( strncmp(buf, "{", MAX_TKN_LEN) == 0 ) {
		t->type = LBRACE;
	} else if ( strncmp(buf, "}", MAX_TKN_LEN) == 0 ) {
		t->type = RBRACE;
	} else if ( strncmp(buf, "*", MAX_TKN_LEN) == 0 ) {
		t->type = ASTERISK;
	} else if ( strncmp(buf, "/", MAX_TKN_LEN) == 0 ) {
		t->type = SLASH;
	} else if ( strncmp(buf, "+", MAX_TKN_LEN) == 0 ) {
		t->type = PLUS;
	} else if ( strncmp(buf, "-", MAX_TKN_LEN) == 0 ) {
		t->type = MINUS;
	} else if ( strncmp(buf, "<-", MAX_TKN_LEN) == 0 ) {
		t->type = LARROW;
	} else if ( strncmp(buf, ";", MAX_TKN_LEN) == 0 ) {
		t->type = SEMICOLON;
	} else if ( strncmp(buf, ".", MAX_TKN_LEN) == 0 ) {
		t->type = PERIOD;
	} else if ( strncmp(buf, ",", MAX_TKN_LEN) == 0 ) {
		t->type = COMMA;
	} else {
		perror("Unrecognized symbol\n");
	}

	return t;
}

void
check_regex_compilation (int rv)
{
	if (rv) {
		perror("Could not compile regex\n");
		exit(1);
	}
}
