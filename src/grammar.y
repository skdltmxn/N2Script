%{
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "grammar.h"


extern int linenum;
extern int yylex();

void yyerror(struct ast_tree *root, const char *s);
%}

%lex-param { struct ast_tree *root }
%parse-param { struct ast_tree *root }

%union {
	struct ast_tree *root;
	struct block *blk;
	struct statement *stmt;
	struct expression *expr;
	int token;
	char *str;
}

%token <token> NUM
%token <str> STR IDENT
%token ASSIGN
%token VAR
%token IF

%type <root> n2script
%type <blk> statements
%type <stmt> statement assignment
%type <expr> expr

%right '='
%left '+' '-'
%left '*' '/'
%%

n2script:   /* empty */
			| statements	{ root->blk = $1; }
			;
			
statements:	
			statements statement	{ add_statement($1, $2); }
			| statement				{ $$ = new_block(); add_statement($$, $1); }
			;
			
statement:
			assignment				{ $$ = $1; }
			;
			
assignment:
			VAR IDENT '=' expr 		{ $$ = new_statement(eval_assign, destroy_assign_stmt); 
									  $$->assign = new_assign_stmt($2, $4); } 	
			;
			
expr:
			expr '+' expr	{ $$ = new_operation(EXP_ADD, $1, $3); }
			| expr '-' expr { $$ = new_operation(EXP_SUB, $1, $3); }
			| expr '*' expr { $$ = new_operation(EXP_MUL, $1, $3); }
			| expr '/' expr { $$ = new_operation(EXP_DIV, $1, $3); }
			| '(' expr ')'	{ $$ = $2; }
			| NUM			{ $$ = new_expression(EXP_NUM, (void *)$1); }
			| STR 			{ $$ = new_expression(EXP_STRING, $1); }
			| IDENT 		{ $$ = new_expression(EXP_IDENT, $1); }
			;

%%

void yyerror(struct ast_tree *root, const char *s)
{
	fprintf(stderr, "%s (line: %d)\n", s, linenum);
	destroy_ast(root);
	exit(-1);
}