/*
 * eval.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __EVAL_H_
#define __EVAL_H_

#define TYPE_CHECK_OK       1
#define TYPE_CHECK_PENDING  0
#define TYPE_CHECK_ERROR    (-1)

extern int evaluate(const struct ast_tree *root);
extern int add_expression(const struct expression *left,
                          const struct expression *right,
                          struct expression *result);
extern int sub_expression(const struct expression *left,
                          const struct expression *right,
                          struct expression *result);
extern int mul_expression(const struct expression *left,
                          const struct expression *right,
                          struct expression *result);
extern int div_expression(const struct expression *left,
                          const struct expression *right,
                          struct expression *result);
extern int eval_assign(const struct statement *stmt);
extern int type_check(const enum expr_type type,
                      const struct expression *left,
                      const struct expression *right);
extern void eval_error(const char *s, ...);

#endif
