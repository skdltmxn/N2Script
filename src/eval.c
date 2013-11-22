/*
 * eval.c
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 * This file contains code for evaluating script
 *
 */

#include <stdio.h>
#include "types.h"
#include "ast.h"
#include "eval.h"

int evaluate(struct ast_tree *root)
{
	struct statement *stmts;
	stmts = root->blk->stmts;

	while (stmts)
	{
		if (!stmts->execute(stmts))
			return 0;

		stmts = stmts->next;
	}

	return 1;
}

struct expression *eval_expression(struct expression *expr,
		struct expression *result)
{
	struct expression *left = NULL;
	struct expression *right = NULL;

	if (!expr || !result)
		return NULL;

	switch (expr->type)
	{
	case EXP_ADD:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value = (void *)((int)left->value + (int)right->value);
		}
		return result;

	case EXP_SUB:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value = (void *)((int)left->value - (int)right->value);
		}
		return result;

	case EXP_MUL:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value = (void *)((int)left->value * (int)right->value);
		}
		return result;

	case EXP_DIV:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value = (void *)((int)left->value / (int)right->value);
		}
		return result;

	case EXP_NUM:
	case EXP_STRING:
		result->value = expr->value;
		result->type = expr->type;
		return expr;

	case EXP_IDENT:
		resolve_var(result);
		return result;
	}

	/* impossible */
	return NULL;
}

int eval_assign(struct statement *stmt)
{
	struct assign_stmt *assign = stmt->assign;
	struct expression result;

	if (!assign)
		return 0;

	if (!eval_expression(assign->expr, &result))
		return 0;

	printf("%s <- %d\n", assign->ident, (int)result.value);
	return 1;
}
