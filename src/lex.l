%{
#include "types.h"
#include "ast.h"
#include "grammar.h"

extern int linenum;
%}

%option noyywrap nounistd
%x COMMENT STRING

%%
[ \t]				;
#[^\n]*\n			{ ++linenum; }
<INITIAL>\[\[\[		{ BEGIN(COMMENT); }
<COMMENT>\n			{ ++linenum; }
<COMMENT>.			;
<COMMENT>\]\]\]		{ BEGIN(INITIAL); }
<INITIAL>\"			{ BEGIN(STRING); }
<STRING>(\\.|[^"])* { yylval.str = strdup(yytext); return STR; }
<STRING>\"			{ BEGIN(INITIAL); }
var					{ return VAR; }
if					{ return IF; }
[+\-*/=()]			{ return yytext[0]; }
[0-9]+				{ yylval.token = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9]* { yylval.str = strdup(yytext); return IDENT; }
\n					{ ++linenum; }
%%