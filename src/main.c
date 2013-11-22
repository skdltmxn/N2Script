/*
 * main.c
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 * This file contains the main and other initialization functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ast.h"
#include "eval.h"

static struct ast_tree *root = NULL;

int linenum = 1;
extern int yyparse();
extern int yylex();

int main(int argc, char **argv)
{
	root = init_ast();
	if (!root)
	{
		fprintf(stderr, "out of memory!\n");
		exit(-1);
	}

	yyparse(root);

	evaluate(root);

	destroy_ast(root);

	return 0;
}
