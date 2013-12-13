/*
 * eval.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __EVAL_H_
#define __EVAL_H_

#define TYPE_CHECK_OK		1
#define TYPE_CHECK_PENDING	0
#define TYPE_CHECK_ERROR	(-1)

extern int evaluate(struct ast_tree *root);
extern int add_expression(struct expression *left,
				struct expression *right, struct expression *result);
extern int sub_expression(struct expression *left,
				struct expression *right, struct expression *result);
extern int mul_expression(struct expression *left,
				struct expression *right, struct expression *result);
extern int div_expression(struct expression *left,
				struct expression *right, struct expression *result);
extern int eval_assign(struct statement *stmt);
extern int type_check(const enum expr_type type,
		struct expression *left, struct expression *right);
extern int resolve_var(struct expression *expr);

#endif
