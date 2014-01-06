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
#include "variable.h"
#include "eval.h"
#include "grammar.h"

extern int yylex();

void yyerror(YYLTYPE *locp, node **n, yyscan_t scanner, const char *s);
%}

%code requires {
 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#define YYSTYPE struct _node*

}

%define api.pure
%locations
%defines
%error-verbose
%lex-param { yyscan_t scanner }
%parse-param { node **root }
%parse-param { yyscan_t scanner }

%token T_INT "integer value (T_INT)"
%token T_DOUBLE "double value (T_DOUBLE)"
%token T_STRING "string value (T_STRING)"
%token T_IDENT "identifier (T_IDENT)"
%token T_IF "if (T_IF)"
%token T_ELSE "else (T_ELSE)"
%token END 0 "end of file"

%right '='

%left '|'
%left '^'
%left '&'
%left '+' '-'
%left '*' '/'
%%

n2script:   /* empty */
            | statements         { *root = $1; }
            ;

statements: statement            { $$ = $1; }
            | statements statement { link_node($1, $2); }
            ;

statement:  '{' statements '}'   { $$ = $2; }
            | ifstmt			 { $$ = $1; }
            | expr               { $$ = $1; }
            ;

ifstmt:     T_IF '(' expr ')' statement
                                 {}
            | T_IF '(' expr ')' statement T_ELSE statement
                                 {}
            ;

expr:       expr '+' expr        { $$ = new_operation(NODE_ADD, $1, $3); }
            | expr '-' expr      { $$ = new_operation(NODE_SUB, $1, $3); }
            | expr '*' expr      { $$ = new_operation(NODE_MUL, $1, $3); }
            | expr '/' expr      { $$ = new_operation(NODE_DIV, $1, $3); }
            | expr '&' expr      { $$ = new_operation(NODE_AND_BIN, $1, $3); }
            | expr '^' expr      { $$ = new_operation(NODE_XOR, $1, $3); }
            | expr '|' expr      { $$ = new_operation(NODE_OR_BIN, $1, $3); }
            | T_IDENT '=' expr   { $$ = new_assign($1, $3); }
            | '(' expr ')'       { $$ = $2; }
            | T_INT              { $$ = $1; }
            | T_DOUBLE           { $$ = $1; }
            | T_STRING           { $$ = $1; }
            | T_IDENT            { $$ = $1; }
            ;

%%

void yyerror(YYLTYPE *locp, node **n, yyscan_t scanner, const char *s)
{
    parse_error(*n, s);
}