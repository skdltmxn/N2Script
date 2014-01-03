/*
 *  ast.h
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __AST_T_
#define __AST_T_

#include "node.h"

extern node *new_assign(node *lhs, node *rhs);
extern node *new_operation(const node_type type, node *left, node *right);
extern void parse_error(node *n, const char *msg);

#endif
