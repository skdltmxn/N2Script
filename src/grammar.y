%{
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "grammar.h"


extern int linenum;
extern int yylex();

void yyerror(YYLTYPE *locp, struct ast_tree *root, yyscan_t scanner, const char *s);
%}

%code requires {
 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 
}

%define api.pure
%locations
%defines
%error-verbose
%lex-param { yyscan_t scanner }
%parse-param { struct ast_tree *root }
%parse-param { yyscan_t scanner }

%union {
	struct ast_tree *root;
	struct block *blk;
	struct statement *stmt;
	struct expression *expr;
	int integer;
	double real;
	byte *str;
}

%token <integer> INTEGER
%token <real> REAL
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
			
statements:	statements statement	{ add_statement($1, $2); }
			| statement		{ $$ = new_block(); add_statement($$, $1); }
			;
			
statement:
			assignment	{ $$ = $1; }
			;
			
assignment:
			VAR IDENT '=' expr	{ $$ = new_statement(eval_assign, destroy_assign_stmt); 
								$$->assign = new_assign_stmt($2, $4); } 	
			;
			
expr:
			expr '+' expr	{ $$ = new_operation(EXP_ADD, $1, $3); }
			| expr '-' expr	{ $$ = new_operation(EXP_SUB, $1, $3); }
			| expr '*' expr	{ $$ = new_operation(EXP_MUL, $1, $3); }
			| expr '/' expr	{ $$ = new_operation(EXP_DIV, $1, $3); }
			| '(' expr ')'	{ $$ = $2; }
			| INTEGER		{ union exp_value v; v.integer = $1; $$ = new_expression(EXP_INTEGER, &v); }
			| REAL			{ union exp_value v; v.real = $1; $$ = new_expression(EXP_FLOAT, &v); }
			| STR 			{ union exp_value v; v.string = $1; $$ = new_expression(EXP_STRING, &v); }
			| IDENT 		{ union exp_value v; v.string = $1; $$ = new_expression(EXP_IDENT, &v); }
			;

%%

void yyerror(YYLTYPE *locp, struct ast_tree *root, yyscan_t scanner, const char *s)
{
	parse_error(root, s);
}