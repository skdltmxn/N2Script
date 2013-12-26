/*
 *  grammar.y
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file defines grammar for N2Script
 *
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "grammar.h"

extern int yylex();

void yyerror(YYLTYPE *locp, struct ast_tree *root, yyscan_t scanner, const char *s);
static struct var_table *parent = NULL, *current = NULL;
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
    struct statement *stmt;
    struct expression *expr;
    int integer;
    double real;
    const char *str;
}

%token <integer> INTEGER
%token <real> REAL
%token <str> STR IDENT
%token IF ELSE

%type <root> n2script
%type <stmt> statements statement assignment ifstmt
%type <expr> expr

%right '='
%left '+' '-'
%left '*' '/'
%%

n2script:   /* empty */
            | { current = root->var_tbl; } statements { root->stmts = $2; }
            ;
            
statements: statements statement { add_statement($1, $2); }
            | statement          { $$ = $1; }
            ;
            
statement:  '{' statements '}'   { $$ = $2; }
            | assignment         { $$ = $1; }
            | ifstmt			 { $$ = $1; }
            ;
            
assignment: IDENT '=' expr       { $$ = new_statement(eval_assign, destroy_assign_stmt); 
                                   $$->assign = new_assign_stmt($1, $3); }     
            ;
            
ifstmt:     IF '(' expr ')' statement 
                                 { $$ = new_statement(eval_if, destroy_if_stmt);
                                   $$->ifstmt = new_if_stmt($3, $5, NULL); }
            | IF '(' expr ')' statement ELSE statement
                                 { $$ = new_statement(eval_if, destroy_if_stmt);
                                   $$->ifstmt = new_if_stmt($3, $5, $7); }
            ;
            
expr:       expr '+' expr        { $$ = new_operation(EXP_ADD, $1, $3); }
            | expr '-' expr      { $$ = new_operation(EXP_SUB, $1, $3); }
            | expr '*' expr      { $$ = new_operation(EXP_MUL, $1, $3); }
            | expr '/' expr      { $$ = new_operation(EXP_DIV, $1, $3); }
            | '(' expr ')'       { $$ = $2; }
            | INTEGER            { union exp_value v; v.integer = $1; $$ = new_expression(EXP_INTEGER, &v, current); }
            | REAL               { union exp_value v; v.real = $1; $$ = new_expression(EXP_REAL, &v, current); }
            | STR                { union exp_value v; v.string = $1; $$ = new_expression(EXP_STRING, &v, current); }
            | IDENT              { union exp_value v; v.string = $1; $$ = new_expression(EXP_IDENT, &v, current); }
            ;

%%

void yyerror(YYLTYPE *locp, struct ast_tree *root, yyscan_t scanner, const char *s)
{
    parse_error(root, s);
}