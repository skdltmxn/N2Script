/*
 *  node.c
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 *  This file manages nodes, the unit of AST
 *
 */

#include <stdlib.h>
#include "types.h"
#include "node.h"
#include "util.h"

node *new_node(node_type type,
               node_value val,
               node_handler handler,
               node_dtor dtor)
{
    node *n = safe_malloc(sizeof(*n));

    n->next = NULL;
    n->child = NULL;
    n->type = type;
    n->val = val;
    n->handler = handler;
    n->dtor = dtor;

    /*printf("node made %p\n", n);*/

    return n;
}

void link_node(node *before, node *after)
{
    node **iter = NULL;

    iter = &before;

    while (*iter) iter = &(*iter)->next;

    *iter = after;
}

void destroy_node(node *n)
{
    if (!n)
        return;

    destroy_node(n->next);

    if (n->dtor)
        n->dtor(n);

    /*printf("node destroyed %p %d\n", n, n->type);*/
    safe_free(n->child);
    safe_free(n);
}