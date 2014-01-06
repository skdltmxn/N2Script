/*
 *  node.h
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __NODE_H_
#define __NODE_H_

typedef union
{
    const byte *sval;
    int ival;
    double dval;
} node_value;

typedef enum
{
    NODE_DOUBLE,
    NODE_INT,
    NODE_STRING,
    NODE_IDENT,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_AND_BIN,
    NODE_OR_BIN,
    NODE_XOR,
    NODE_AND_LOG,
    NODE_OR_LOG,
    NODE_ASSIGN,
    NODE_IF
} node_type;

typedef struct _node
{
    struct _node *next;
    struct _node **child;
    int (* handler)(const struct _node *, struct _node *);
    void (* dtor)(struct _node *);
    node_type type;
    node_value val;
} node;

typedef int (* node_handler)(const node *, node *);
typedef void (* node_dtor)(node *);

/*  */
#define NODE_LEFT(n)   (n)->child[0]
#define NODE_RIGHT(n)  (n)->child[1]

#define NODE_LHS(n) (n)->child[0]
#define NODE_RHS(n) (n)->child[1]

extern node *new_node(node_type type,
                      node_value val,
                      node_handler handler,
                      node_dtor dtor);
extern void link_node(node *before, node *after);
extern void destroy_node(node *n);

#endif