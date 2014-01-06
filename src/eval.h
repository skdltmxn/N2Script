/*
 * eval.h
 *
 *   Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __EVAL_H_
#define __EVAL_H_

#define TYPE_CHECK_OK       1
#define TYPE_CHECK_PENDING  0
#define TYPE_CHECK_ERROR    (-1)

extern int evaluate(node *nodes);
extern int add_expression(const node *left, const node *right, node *result);
extern int sub_expression(const node *left, const node *right, node *result);
extern int mul_expression(const node *left, const node *right, node *result);
extern int div_expression(const node *left, const node *right, node *result);
extern int eval_binop(const node *self, node *result);
extern int eval_refer(const node *, node *);
extern int eval_assign(const node *, node *);
extern int eval_if(const node *, node *);
extern int type_check(const node_type type, const node *left, const node *right);
extern void eval_error(const char *s, ...);

#endif
