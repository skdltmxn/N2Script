/*
 *  main.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file contains the main and other initialization functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "resource.h"
#include "string.h"

static struct ast_tree *root = NULL;

extern int yylex_init(yyscan_t scanner);
extern int yyparse(struct ast_tree *root, yyscan_t scanner);
extern int yylex_destroy(yyscan_t scanner);
extern void destroy_string_buffer();

static void init_rsrc()
{
    init_rsrc_pool(RSRC_STRING, add_string, search_string, destroy_rsrc_string);
}

struct ast_tree *get_ast_root()
{
    return root;
}

static int parse_script()
{
    yyscan_t scanner;

    if (yylex_init(&scanner))
        return 0;

    root->scanner = scanner;
    if (yyparse(root, scanner))
        return 0;

    destroy_string_buffer();
    yylex_destroy(scanner);

    return 1;
}

/*
 * Release all resources
 */
void destroy_all()
{
    destroy_string_buffer();
    destroy_rsrc();
    destroy_ast(root);
}

int main(int argc, char **argv)
{
    /* Default encoding is UTF-8 */	
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "");
#endif

    root = init_ast();
    if (!root)
    {
        fprintf(stderr, "out of memory!\n");
        exit(-1);
    }

    init_rsrc();

    if (!parse_script())
        return 1;

    evaluate(root->stmts);
    destroy_all();

    return 0;
}
