/*
 *  ast.c
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 *  This file contains code for construction/destruction of the AST tree
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "util.h"
#include "grammar.h"

void destroy_assign(node *n)
{
    destroy_node(NODE_LHS(n));
    destroy_node(NODE_RHS(n));
}

node *new_assign(node *lhs, node *rhs)
{
    node *n = new_node(NODE_ASSIGN, node_dummy_value(), eval_assign, destroy_assign);

    n->child = (node **)safe_malloc(sizeof(n) * 2);
    NODE_LHS(n) = lhs;
    NODE_RHS(n) = rhs;

    return n;
}

void destroy_operation(node *n)
{
    if (n->child)
    {
        destroy_node(NODE_LEFT(n));
        destroy_node(NODE_RIGHT(n));
    }
}

node *new_operation(const node_type type, node *left, node *right)
{
    node *n = NULL;
    int check = TYPE_CHECK_OK;

    n = new_node(type, node_dummy_value(), NULL, destroy_operation);

    check = type_check(type, left, right);

    /* cannot evaluate for now */
    if (check == TYPE_CHECK_PENDING)
    {
        n->child = (node **)safe_malloc(sizeof(n) * 2);
        NODE_LEFT(n) = left;
        NODE_RIGHT(n) = right;

        return n;
    }
    /* syntax error */
    else if (check == TYPE_CHECK_ERROR)
        parse_error(right, "type mismatch");

    /* evaluate as much as possible */
    if (type == NODE_ADD)
    {
        add_expression(left, right, n);
    }
    else if (type == NODE_SUB)
    {
        sub_expression(left, right, n);
    }
    else if (type == NODE_MUL)
    {
        mul_expression(left, right, n);
    }
    else if (type == NODE_DIV)
    {
        div_expression(left, right, n);
    }

    destroy_node(left);
    destroy_node(right);

    return n;
}

extern int yylex_destroy(yyscan_t scanner);
void parse_error(node *n, const char *msg)
{
    fprintf(stderr, "Parse error: %s\n", msg);
    destroy_all();
    exit(-3);
}
