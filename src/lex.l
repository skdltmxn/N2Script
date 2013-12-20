/*
 *  lex.l
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file defines rules for lexical analyzer
 *
 */
 
%{
#include "types.h"
#include "ast.h"
#include "grammar.h"
#include "util.h"
#include "resource.h"

#define BUFFER_SIZE	4096
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
			yylloc->last_column += yyget_leng(yyscanner);
			
void lerror(yyscan_t scanner, const char *s);
static int init_string_buffer(const ui32 size);
static int add_string_buffer_byte(byte c);
static int add_string_buffer(byte *p, const ui32 len);
static byte *get_string_buffer();
void destroy_string_buffer();

static byte *_string_buffer = NULL;
static ui32 _pos = 0;
static ui32 _size = 0;
%}

%option noyywrap nounistd reentrant bison-bridge bison-locations yylineno
%x COMMENT STRING

%%
[ \t]				;
#[^\n]*[\n]			;
#.*					;
<INITIAL>\[\[\[		BEGIN(COMMENT);
<COMMENT>\n			{ yylloc->first_column = yylloc->last_column = 1; }
<COMMENT>.			;
<COMMENT>\]\]\]		BEGIN(INITIAL);
<INITIAL>\"			{ if (!init_string_buffer(BUFFER_SIZE))
						lerror(yyscanner, "Failed to initialize string buffer");
						
					  BEGIN(STRING); 
					}
<STRING>\n			lerror(yyscanner, "Unterminated string found");
<STRING><<EOF>>		lerror(yyscanner, "Unterminated string found");
<STRING>[^\\\n"]*	{ add_string_buffer(yytext, strlen(yytext)); }
<STRING>\\[0-7]{3}	{
						int ret = 0;
						int octal = 0;
						octal = (yytext[1] - '0') * 64 +
								(yytext[2] - '0') * 8 +
								(yytext[3] - '0');
						
						if (octal > 255)
							lerror(yyscanner, "Too big octal character");
						
						ret = add_string_buffer_byte(octal);
						if (!ret)
							lerror(yyscanner, "Failed to parse string");
					}
<STRING>\\x[0-9a-fA-F]{2} {
						int ret = 0;
						byte hex = string_to_hex(&yytext[2]);
						
						ret = add_string_buffer_byte(hex);
						if (!ret)
							lerror(yyscanner, "Failed to parse string");
					}
<STRING>\\u[0-9a-fA-F]{4} {
						int ret = 0;
						char utf8[4];
						ui16 unicode = 0;
						int len = 0;
						
						unicode = (string_to_hex(&yytext[2]) << 8) |
								string_to_hex(&yytext[4]);
						
						len = encode_utf8(utf8, unicode);
						if (!len)
							lerror(yyscanner, "Invalid unicode code points found");
						
						ret = add_string_buffer(utf8, len);
						if (!ret)
							lerror(yyscanner, "Failed to parse string");
					}
<STRING>\\.			{
						int ret = 0;
						switch (yytext[1])
						{
						case 'n':
							ret = add_string_buffer_byte('\n');
							break;
							
						case 't':
							ret = add_string_buffer_byte('\t');
							break;
							
						case 'r':
							ret = add_string_buffer_byte('\r');
							break;
							
						case '"':
							ret = add_string_buffer_byte('"');
							break;
						
						case '0':
							ret = add_string_buffer_byte('\0');
							break;
												
						default:
							ret = add_string_buffer_byte(yytext[1]);
							break;
						}
						
						if (!ret)
							lerror(yyscanner, "Failed to parse string");
					}
<STRING>\"			{
						yytext[yyleng - 1] = '\0';
						yylval->str = register_rsrc_string(get_string_buffer()); BEGIN(INITIAL); return STR; }
if					{ return IF; }
[0-9]+\.[0-9]+		{ yylval->real = atof(yytext); return REAL; }
[0-9]+				{ yylval->integer = atoi(yytext); return INTEGER; }
[a-zA-Z_][a-zA-Z0-9]* { yylval->str = register_rsrc_string(yytext); return IDENT; }
[+\-*/=(){}]		{ return yytext[0]; }
\n					{ yylloc->first_column = yylloc->last_column = 1; }
%%

static int init_string_buffer(const ui32 size)
{
	if (_string_buffer)
		return 1;
		
	_string_buffer = (byte *)malloc(size);
	
	if (!_string_buffer)
		return 0;
	
	_size = size;
	return 1;
}

static int add_string_buffer_byte(byte c)
{
	if (!_string_buffer)
		return 0;
	
	/* buffer is full */
	if (_pos + 2 >= _size)
	{
		_string_buffer = (byte *)realloc(_string_buffer,
									_size * 2);
		
		if (!_string_buffer)
			return 0;
		
		_size *= 2;
	}
	
	*(_string_buffer + _pos++) = c;
	*(_string_buffer + _pos)   = '\0';
	
	return 1;
}

static int add_string_buffer(byte *p, const ui32 len)
{
	if (!_string_buffer)
		return 0;
	
	if (_pos + len + 1 >= _size)
	{
		_string_buffer = (byte *)realloc(_string_buffer,
									_pos + len + 1);
		
		if (!_string_buffer)
			return 0;
		
		_size = _pos + len + 1;
	}
	
	memcpy(_string_buffer + _pos, p, len);
	_pos += len;
	*(_string_buffer + _pos)   = '\0';
	
	return 1;
}

static byte *get_string_buffer()
{
	if (!_string_buffer)
		return NULL;
	
	*(_string_buffer + _pos)   = '\0';	
	_pos = 0;
	return _string_buffer;
}

void destroy_string_buffer()
{
	safe_free(_string_buffer);
}

void lerror(yyscan_t scanner, const char *s)
{
	YYLTYPE *loc = yyget_lloc(scanner);
	
	fprintf(stderr, "%s (line %d, col %d)\n", s, loc->first_line, loc->last_column - 1);
	destroy_all();
	yylex_destroy(scanner);
	exit(-2);
}