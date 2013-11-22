/*
 * eval.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __EVAL_H_
#define __EVAL_H_

extern int evaluate(struct ast_tree *root);
extern int eval_assign(struct statement *stmt);

extern int resolve_var(struct expression *expr);

#endif
