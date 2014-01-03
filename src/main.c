/*
 *  main.c
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 *  This file contains the main and other initialization functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "types.h"
#include "node.h"
#include "eval.h"
#include "resource.h"
#include "string.h"
#include "grammar.h"
#include "variable.h"

extern int yylex_init(yyscan_t scanner);
extern int yylex_destroy(yyscan_t scanner);
extern void destroy_string_buffer();

static node *root = NULL;

static void init_rsrc()
{
    init_var_table();
    init_rsrc_pool(RSRC_STRING, add_string, search_string, destroy_rsrc_string);
}

static int parse_script(node **root)
{
    yyscan_t scanner;

    if (yylex_init(&scanner))
    {
        fprintf(stderr, "failed to init scanner\n");
        return 0;
    }

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
    destroy_var_table();
    destroy_node(root);
}

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    init_rsrc();

    if (!parse_script(&root))
        return 1;

    evaluate(root);
    destroy_all();

    return 0;
}
