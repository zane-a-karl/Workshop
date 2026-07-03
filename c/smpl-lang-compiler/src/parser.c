#include "../hdr/parser.h"

int node_ctr = 0;

// Tkns that signal a stmt beginning
const enum TokenType stmt_initials[] = {LET,
																				CALL,
																				IF,
																				WHILE,
																				RETURN};
const int num_stmt_initials =
	sizeof(stmt_initials)/sizeof(enum TokenType);

// Tkns that signal an factor starting
const enum TokenType fctr_initials[] = {IDENT,
																				NUMBER,
																				LPAREN,
																				CALL};
const int num_fctr_initials =
	sizeof(fctr_initials)/sizeof(enum TokenType);

const char *psr_sectors[] = {
	"COMPUTATION",
	"VAR_DECL",
	"TYPE_DECL",
	"FUNC_DECL",
	"FORMAL_PARAM",
	"FUNC_BODY",
	"STAT_SEQ",
	"STMT",
	"ASSIGNMENT",
	"DESIGNATOR",
	"EXPRESSION",
	"TERM",
	"FACTOR",
	"FUNC_CALL",
	"IF_STMT",
	"RELATION",
	"WHILE_STMT",
	"RETURN_STMT"
};

extern const char *tkn_types[];

struct Parser *
new_parser (char *filename)
{
	struct Parser *p   = calloc(1, sizeof(struct Parser));
	// Allocs heap memory to lexer
	p->lxr      = new_lexer(filename);
	p->curr_tkn = lex_next_tkn(p->lxr);
	p->fwarn    = fopen("logs/warn.txt", "w+");
	check_fopen(p->fwarn);

	return p;
}

void
free_parser (struct Parser **p)
{
	free_lexer(&((*p)->lxr));
	// p->curr_tkn is free'd with the lexer
	fclose((*p)->fwarn);
	free(*p);
}

bool
peek_tkn (enum TokenType t,
					struct Parser *p)
{
	return p->curr_tkn->tkn->type == t;
}

bool
peek_stmt_initials (struct Parser *p)
{
	for (int i = 0; i < num_stmt_initials; ++i) {
		if (p->curr_tkn->tkn->type == stmt_initials[i])
			return true;
	}
	return false;
}

bool
peek_fctr_initials (struct Parser *p)
{
	for (int i = 0; i < num_fctr_initials; ++i) {
		if (p->curr_tkn->tkn->type == fctr_initials[i])
			return true;
	}
	return false;
}

void
throw_parser_error (enum TokenType t,
										struct Parser *p,
										enum ParserSector ps)
{
	printf("Parser Error: ");
	printf("On (line:%d,col:%d) ", p->lxr->line, p->lxr->col);
	printf("expected '%s' but found '%s' ",
				 tkn_types[t],
				 p->curr_tkn->tkn->raw);
	printf("within '%s'\n", psr_sectors[ps]);
	exit(1);
}

void
throw_parser_warning (enum TokenType t,
											struct Parser *p,
											enum ParserSector ps)
{
	fprintf(p->fwarn,
					"Parser Warning (Possible Extraneous Character):\n On (line:%d,col:%d) expected '%s' but found '%s' within '%s'\n",
					p->lxr->line,
					p->lxr->col,
					tkn_types[t],
					p->curr_tkn->tkn->raw,
					psr_sectors[ps]);
}

void
consume_tkn (enum TokenType t,
						 struct Parser *p,
						 enum ParserSector ps)
{
	if ( peek_tkn(t, p) ) {
		p->curr_tkn = lex_next_tkn(p->lxr);
	} else {
		throw_parser_error(t, p, ps);
	}
}

struct Ast *
parse (struct Parser *p,
			 Agraph_t      *tlg)
{
	// ast->graph created here
	struct Ast *ast = new_ast(tlg);
	ast->root       = smpl_computation(p, ast->graph);

	return ast;
}

// computation = "main" { varDecl } { funcDecl }
// "{" statSequence "}" "."
struct AstNode *
smpl_computation (struct Parser *p,
									Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, CMPTN);
	// c->vdl, c->fdl calloc'd here
	node->computation = new_computation();
	consume_tkn(MAIN, p, COMPUTATION);
	struct AstNodeList *var_decl;
	while ( peek_tkn(VAR, p) ||	peek_tkn(ARRAY, p) ) {

		var_decl = smpl_var_decl(p, g);
		concat_ast_list(node->computation->var_decls, &var_decl);
	}
	while ( peek_tkn(FUNCTION, p) || peek_tkn(VOID, p) ) {
		push_ast_node(node->computation->func_decls,
									smpl_func_decl(p, g));
	}
	consume_tkn(LBRACE, p, COMPUTATION);
	node->computation->stmts = smpl_stat_sequence(p, g);
	consume_tkn(RBRACE, p, COMPUTATION);
	consume_tkn(PERIOD, p, COMPUTATION);

	return node;
}

// varDecl = typeDecl ident { "," ident } ";"
struct AstNodeList *
smpl_var_decl (struct Parser *p,
							 Agraph_t      *g)
{
	struct AstNodeList *nodes = new_ast_node_list();
	push_ast_node(nodes, new_ast_node(g, node_ctr++, VARDECL));
	nodes->head->var_decl = new_vd();
	nodes->head->var_decl->dims = smpl_type_decl(p, g);
	nodes->head->var_decl->ident = smpl_ident(p, g, VAR_DECL);
	struct AstNode *i;

	while ( peek_tkn(COMMA, p) ) {

		consume_tkn(COMMA, p, VAR_DECL);
		i = new_ast_node(g, node_ctr++, VARDECL);
		push_ast_node(nodes, i);
		i->var_decl = new_vd();
		i->var_decl->ident = smpl_ident(p, g, VAR_DECL);
		i->var_decl->dims =
			deep_copy_ast_node_list(nodes->head->var_decl->dims);
	}
	consume_tkn(SEMICOLON, p, VAR_DECL);

	return nodes;
}

// typeDecl = "var" |
// "array" "[" number "]" { "[" number "]"}
// Note: we only have to handle integer types and their
//  arrays because of this we can have the parser return
//  the list of dimensions of the identifier. For scalars
//  we would just  have an empty list, i.e.
//  list->head = NULL.
struct AstNodeList *
smpl_type_decl (struct Parser *p,
								Agraph_t      *g)
{
	struct AstNodeList *nodes = new_ast_node_list();
	if ( peek_tkn(VAR, p) ) {

		consume_tkn(VAR, p, TYPE_DECL);
		nodes->head = NULL; // i.e. a scalar

	} else if ( peek_tkn(ARRAY, p) ) {

		consume_tkn(ARRAY, p, TYPE_DECL);
		do {

			consume_tkn(LBRACKET, p, TYPE_DECL);
			push_ast_node(nodes, smpl_number(p, g, TYPE_DECL));
			consume_tkn(RBRACKET, p, TYPE_DECL);
		} while ( peek_tkn(LBRACKET, p) );

	} else {
		throw_parser_error(p->curr_tkn->tkn->type, p, TYPE_DECL);
	}

	return nodes;
}

struct AstNode *
smpl_number (struct Parser *p,
						 Agraph_t      *g,
						 enum ParserSector ps)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, NUM);
	node->number = new_number();
	node->number->val = p->curr_tkn->tkn->val;
	consume_tkn(NUMBER, p, ps);

	return node;
}

struct AstNode *
smpl_ident (struct Parser *p,
						Agraph_t      *g,
						enum ParserSector ps)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, IDNT);
	// ident->name calloc'd here
	node->identifier = new_ident();
	for (int i = 0; i < MAX_VAR_NAME_LEN; ++i) {
		node->identifier->name[i] = p->curr_tkn->tkn->raw[i];
	}
	consume_tkn(IDENT, p, ps);

	return node;
}

// funcDecl =
// [ "void" ] "function"
// ident formalParam ";" funcBody ";"
struct AstNode *
smpl_func_decl (struct Parser *p,
								Agraph_t      *g)
{
	struct AstNode *node =
		new_ast_node(g, node_ctr++, FUNCDECL);
	node->func_decl = new_fd();
	if ( peek_tkn(VOID, p) ) {
		consume_tkn(VOID, p, FUNC_DECL);
		node->func_decl->is_void = true;
	}
	consume_tkn(FUNCTION, p, FUNC_DECL);
	node->func_decl->fn_ident =
		smpl_ident(p, g, FUNC_DECL);

	node->func_decl->param_idents =
		smpl_formal_param(p, g);

	consume_tkn(SEMICOLON, p, FUNC_DECL);
	node->func_decl->body =
		smpl_func_body(p, g);
	consume_tkn(SEMICOLON, p, FUNC_DECL);

	return node;
}

// formalParam = "(" [ident { "," ident }] ")"
struct AstNodeList *
smpl_formal_param (struct Parser *p,
									 Agraph_t      *g)
{
	struct AstNodeList *nodes = new_ast_node_list();
	consume_tkn(LPAREN, p, FORMAL_PARAM);
	if ( peek_tkn(IDENT, p) ) {

		push_ast_node(nodes,
									smpl_ident(p, g, FORMAL_PARAM));
		while ( peek_tkn(COMMA, p) ) {

			consume_tkn(COMMA, p, FORMAL_PARAM);
			push_ast_node(nodes,
										smpl_ident(p, g, FORMAL_PARAM));
		} // endwhile
	} // endif
	consume_tkn(RPAREN, p, FORMAL_PARAM);

	return nodes;
}

// funcBody = { varDecl } "{" [ statSequence ] "}"
struct FuncBody *
smpl_func_body (struct Parser *p,
								Agraph_t      *g)
{
	// fb->local_vars calloc'd here
	struct FuncBody *fb = new_func_body();
	struct AstNodeList *var_decl;
	while ( peek_tkn(VAR, p) || peek_tkn(ARRAY, p) ) {

		var_decl = smpl_var_decl(p, g);
		concat_ast_list(fb->local_vars, &var_decl);
	}
	consume_tkn(LBRACE, p, FUNC_BODY);
	fb->stmts = smpl_stat_sequence(p, g);
	consume_tkn(RBRACE, p, FUNC_BODY);

	return fb;
}

// statSequence = statement { ";" statement } [ ";" ]
struct AstNodeList *
smpl_stat_sequence (struct Parser *p,
										Agraph_t      *g)
{
	struct AstNodeList *nodes = new_ast_node_list();
	push_ast_node(nodes, smpl_statement(p, g));
	while ( peek_tkn(SEMICOLON, p) ) {

		consume_tkn(SEMICOLON, p, STAT_SEQ);
		if ( peek_stmt_initials(p) ) {
			push_ast_node(nodes, smpl_statement(p, g));
		}
	}

	return nodes;
}

// statement =
// assignment | "void" funcCall | ifStatement |
// whileStatement | returnStatement
struct AstNode *
smpl_statement (struct Parser *p,
								Agraph_t      *g)
{
	struct AstNode *node = NULL;
	if ( peek_tkn(LET, p) ) {
		node = smpl_assignment(p, g);
	} else if ( peek_tkn(CALL, p) ) {
		node = smpl_func_call(p, g);
	} else if ( peek_tkn(IF, p) ) {
		node = smpl_if_statement(p, g);
	} else if ( peek_tkn(WHILE, p) ) {
		node = smpl_while_statement(p, g);
	} else if ( peek_tkn(RETURN, p) ) {
		node = smpl_return_statement(p, g);
	} else {
		throw_parser_warning(p->curr_tkn->tkn->type, p, STMT);
	}

	return node;
}

// assignment = "let" designator "<-" expression
struct AstNode *
smpl_assignment (struct Parser *p,
								 Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, ASSMT);
	node->assignment = new_assignment();
	consume_tkn(LET, p,	ASSIGNMENT);
	node->assignment->lhs = smpl_designator(p, g);
	consume_tkn(LARROW, p, ASSIGNMENT);
	node->assignment->rhs = smpl_expression(p, g);

	return node;
}

// designator = ident{ "[" expression "]" }
struct AstNode *
smpl_designator (struct Parser *p,
								 Agraph_t      *g)
{
	struct AstNode *node = smpl_ident(p, g, DESIGNATOR);
	struct AstNode *tmp;
	bool flag = true;
	while ( peek_tkn(LBRACKET, p) ) {

		consume_tkn(LBRACKET, p, DESIGNATOR);
		if ( flag ) {
			tmp = node;
			node = new_ast_node(g, node_ctr++, ARRACC);
			node->arr_acc = new_array_access();
			node->arr_acc->ident = tmp;
			flag = false;
		}
		push_ast_node(node->arr_acc->indices,
									smpl_expression(p, g));
		consume_tkn(RBRACKET, p, DESIGNATOR);
	}
	return node;
}

// expression = term {("+" | "-") term}
struct AstNode *
smpl_expression (struct Parser *p,
								 Agraph_t      *g)
{
	struct AstNode *node = smpl_term(p, g);
	struct AstNode *tmp;
	while ( peek_tkn(PLUS, p) || peek_tkn(MINUS, p) ) {

		tmp = node;
		node = new_ast_node(g, node_ctr++, BINOP);
		node->bin_op = new_bin_op();
		assert(tmp != node);
		node->bin_op->opa = tmp;

		snprintf(node->bin_op->op, 2, "%s", peek_tkn(PLUS, p) ? "+" : "-");
		consume_tkn(p->curr_tkn->tkn->type, p, EXPRESSION);
		node->bin_op->opb = smpl_term(p, g);
	}

	return node;
}

// term = factor {("*" | "/") factor}
struct AstNode *
smpl_term (struct Parser *p,
					 Agraph_t      *g)
{
	struct AstNode *node = smpl_factor(p, g);
	struct AstNode *tmp;
	while ( peek_tkn(ASTERISK, p) || peek_tkn(SLASH, p) ) {

		tmp = node;
		node = new_ast_node(g, node_ctr++, BINOP);
		node->bin_op = new_bin_op();
		node->bin_op->opa = tmp;

		snprintf(node->bin_op->op, 2, "%s", peek_tkn(ASTERISK, p) ? "*" : "/");
		consume_tkn(p->curr_tkn->tkn->type, p, EXPRESSION);
		node->bin_op->opb = smpl_factor(p, g);
	}

	return node;
}

// factor = designator | number | "(" expression ")" |
// funcCall
// NOTE: only "non-void" fns may be called here.
struct AstNode *
smpl_factor (struct Parser *p,
						 Agraph_t      *g)
{
	struct AstNode *node = NULL;
	if ( peek_tkn(IDENT, p) ) {
		node = smpl_designator(p, g);
	} else if ( peek_tkn(NUMBER, p) ) {
		node = smpl_number(p, g, FACTOR);
	} else if ( peek_tkn(LPAREN, p) ) {

		consume_tkn(LPAREN, p, FACTOR);
		node = smpl_expression(p, g);
		consume_tkn(RPAREN, p, FACTOR);
	} else if ( peek_tkn(CALL, p) ) {
		node = smpl_func_call(p, g);
	} else if ( peek_tkn(SEMICOLON, p) ) {
		//empty expression
	} else {
		throw_parser_error(p->curr_tkn->tkn->type, p, FACTOR);
	}

	return node;
}

// funcCall =
// "call" ident [ "(" [expression
// { "," expression } ] ")" ]
// fns without params don't need "()" but can have them!
struct AstNode *
smpl_func_call (struct Parser *p,
								Agraph_t      *g)
{
	struct AstNode *node =
		new_ast_node(g, node_ctr++, FUNCCALL);
	// fc->args calloc'd here
	node->func_call = new_func_call();
	consume_tkn(CALL, p, FUNC_CALL);
	node->func_call->ident = smpl_ident(p, g, FUNC_CALL);
	if ( peek_tkn(LPAREN, p) ) {

		consume_tkn(LPAREN, p, FUNC_CALL);
		if ( peek_fctr_initials(p) ){

			push_ast_node(node->func_call->args,
										smpl_expression(p, g));
			while ( peek_tkn(COMMA, p) ) {

				consume_tkn(COMMA, p, FUNC_CALL);
				push_ast_node(node->func_call->args,
											smpl_expression(p, g));
			}
		}
		consume_tkn(RPAREN, p, FUNC_CALL);
	}
	return node;
}

// relation = expression relOp expression
struct AstNode *
smpl_relation (struct Parser *p,
							 Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, BINOP);
	// bo->op calloc'd here
	node->bin_op = new_bin_op();
	node->bin_op->opa = smpl_expression(p, g);
	if ( peek_tkn(OP_INEQ, p) ||
			 peek_tkn(OP_EQ, p) ||
			 peek_tkn(OP_LT, p) ||
			 peek_tkn(OP_LE, p) ||
			 peek_tkn(OP_GT, p) ||
			 peek_tkn(OP_GE, p) ) {

		// the op can only be two chars
		strncpy(node->bin_op->op, p->curr_tkn->tkn->raw, 2);
		consume_tkn(p->curr_tkn->tkn->type, p, RELATION);
	}
	node->bin_op->opb = smpl_expression(p, g);

	return node;
}

// ifStatement =
// "if" relation "then"
// statSequence [ "else" statSequence ] "fi"
struct AstNode *
smpl_if_statement (struct Parser *p,
									 Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, IFSTMT);
	node->if_stmt = new_if_stmt();
	consume_tkn(IF, p, IF_STMT);
	node->if_stmt->condition = smpl_relation(p, g);
	consume_tkn(THEN, p, IF_STMT);
	node->if_stmt->then_stmts = smpl_stat_sequence(p, g);
	if ( peek_tkn(ELSE, p) ) {

		consume_tkn(ELSE, p, IF_STMT);
		node->if_stmt->else_stmts = smpl_stat_sequence(p, g);
	}
	consume_tkn(FI, p, IF_STMT);

	return node;
}

// whileStatement = "while" relation "do"
// StatSequence "od"
struct AstNode *
smpl_while_statement (struct Parser *p,
											Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, WHSTMT);
	node->while_stmt = new_while_stmt();
	consume_tkn(WHILE, p, WHILE_STMT);
	node->while_stmt->condition =
		smpl_relation(p, g);
	consume_tkn(DO, p, WHILE_STMT);
	node->while_stmt->do_stmts  =
		smpl_stat_sequence(p, g);
	consume_tkn(OD, p, WHILE_STMT);

	return node;
}

// returnStatement = "return" [ expression ]
struct AstNode *
smpl_return_statement (struct Parser *p,
											 Agraph_t      *g)
{
	struct AstNode *node = new_ast_node(g, node_ctr++, RESTMT);
	node->ret_stmt = new_return_stmt();
	consume_tkn(RETURN, p, RETURN_STMT);
	node->ret_stmt->ret_val = smpl_expression(p, g);

	return node;
}
