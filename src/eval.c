/*
 *  eval.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file contains code for evaluating script
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "util.h"

#define TYPE_NUM(expr)	\
	((expr)->type == EXP_INTEGER || (expr)->type == EXP_REAL)

int evaluate(struct ast_tree *root)
{
	struct statement *stmts;

	if (!root->blk)
		return 1;

	stmts = root->blk->stmts;

	while (stmts)
	{
		if (!stmts->execute(stmts))
			return 0;

		stmts = stmts->next;
	}

	return 1;
}

/*
 * 1) number + number
 * 2) string + string
 */
int add_expression(struct expression *left,
		struct expression *right, struct expression *result)
{
	if (!left || !right || !result)
		return 0;

	/* STR + STR */
	if (left->type == EXP_STRING && right->type == EXP_STRING)
	{
		result->type = EXP_STRING;
		result->value.string = 
			string_concat(left->value.string, right->value.string);

		return 1;
	}
	/* NUM + NUM */
	else if ((left->type == EXP_INTEGER || left->type == EXP_REAL) &&
			  (right->type == EXP_INTEGER || right->type == EXP_REAL))
	{
		/* FLOAT + ? = FLOAT */
		if (left->type == EXP_REAL || right->type == EXP_REAL)
		{
			result->type = EXP_REAL;
			result->value.real =
				((left->type == EXP_REAL) ? left->value.real : left->value.integer) +
				((right->type == EXP_REAL) ? right->value.real : right->value.integer);

			return 1;
		}
		/* INT + INT = INT */
		else
		{
			result->type = EXP_INTEGER;
			result->value.integer = left->value.integer + right->value.integer;

			return 1;
		}
	}

	return 0;
}

int sub_expression(struct expression *left,
		struct expression *right, struct expression *result)
{
	if ((left->type == EXP_INTEGER || left->type == EXP_REAL) &&
		(right->type == EXP_INTEGER || right->type == EXP_REAL))
	{
		/* FLOAT + ? = FLOAT */
		if (left->type == EXP_REAL || right->type == EXP_REAL)
		{
			result->type = EXP_REAL;
			result->value.real =
				((left->type == EXP_REAL) ? left->value.real : left->value.integer) -
				((right->type == EXP_REAL) ? right->value.real : right->value.integer);

			return 1;
		}
		/* INT + INT = INT */
		else
		{
			result->type = EXP_INTEGER;
			result->value.integer = left->value.integer - right->value.integer;

			return 1;
		}
	}

	return 0;
}

int mul_expression(struct expression *left,
		struct expression *right, struct expression *result)
{
	if (!left || !right || !result)
			return 0;

	/* INTEGER * STR */
	if ((left->type == EXP_INTEGER && right->type == EXP_STRING) ||
		 (left->type == EXP_STRING && right->type == EXP_INTEGER))
	{
		int i;
		int count = (left->type == EXP_INTEGER) ?
				left->value.integer : right->value.integer;
		char *str = (left->type == EXP_STRING) ?
				left->value.string : right->value.string;
		ui32 len = string_length(str);

		result->type = EXP_STRING;
		result->value.string = (char *)malloc((len * count + 1) * sizeof(char));

		for (i = 0; i < count; ++i)
			string_copy(result->value.string + (i * len), str, len);

		return 1;
	}
	/* NUM + NUM */
	else if ((left->type == EXP_INTEGER || left->type == EXP_REAL) &&
			  (right->type == EXP_INTEGER || right->type == EXP_REAL))
	{
		/* FLOAT + ? = FLOAT */
		if (left->type == EXP_REAL || right->type == EXP_REAL)
		{
			result->type = EXP_REAL;
			result->value.real =
					((left->type == EXP_REAL) ? left->value.real : left->value.integer) *
					((right->type == EXP_REAL) ? right->value.real : right->value.integer);

			return 1;
		}
		/* INT + INT = INT */
		else
		{
			result->type = EXP_INTEGER;
			result->value.integer =
					left->value.integer * right->value.integer;

			return 1;
		}
	}

	return 0;
}

int div_expression(struct expression *left,
		struct expression *right, struct expression *result)
{
	if (!left || !right || !result)
			return 0;

	if ((left->type == EXP_INTEGER || left->type == EXP_REAL) &&
		(right->type == EXP_INTEGER || right->type == EXP_REAL))
	{
		/* FLOAT / ? = FLOAT */
		if (left->type == EXP_REAL || right->type == EXP_REAL)
		{
			result->type = EXP_REAL;
			result->value.real =
				((left->type == EXP_REAL) ? left->value.real : left->value.integer) /
				((right->type == EXP_REAL) ? right->value.real : right->value.integer);

			return 1;
		}
		/* INT / INT = INT */
		else
		{
			result->type = EXP_INTEGER;
			result->value.integer = left->value.integer / right->value.integer;

			return 1;
		}
	}

	return 0;
}

/*
 * return
 *   1: ok
 *   0: pending
 *   -1: error
 */
int type_check(const enum expr_type type,
		struct expression *left, struct expression *right)
{
	if (!left || !right)
		return TYPE_CHECK_ERROR;

	if (left->type == EXP_IDENT || right->type == EXP_IDENT)
		return TYPE_CHECK_PENDING;

	/* check for primitive types */
	switch (type)
	{
	/*
	 * string + string
	 * number + number
	 */
	case EXP_ADD:
		if (left->type == EXP_STRING && right->type == EXP_STRING)
			break;
		if (left->type == EXP_STRING && right->type != EXP_STRING)
			return TYPE_CHECK_ERROR;
		if (!TYPE_NUM(left) || !TYPE_NUM(right))
			return TYPE_CHECK_ERROR;
		break;

	/* number and number */
	case EXP_DIV:
	case EXP_SUB:
		if (!TYPE_NUM(left) || !TYPE_NUM(right))
			return TYPE_CHECK_ERROR;
		break;

	/*
	 * string * integer
	 * number * number
	 */
	case EXP_MUL:
		if (left->type == EXP_STRING && right->type != EXP_INTEGER)
			return TYPE_CHECK_ERROR;
		if (left->type == EXP_REAL && right->type == EXP_STRING)
			return TYPE_CHECK_ERROR;
		break;

	default:
		return TYPE_CHECK_ERROR;
	}

	return TYPE_CHECK_OK;
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
			result->value.integer = left->value.integer + right->value.integer;
		}
		return result;

	case EXP_SUB:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value.integer = left->value.integer - right->value.integer;
		}
		return result;

	case EXP_MUL:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value.integer = left->value.integer * right->value.integer;
		}
		return result;

	case EXP_DIV:
		left = eval_expression(expr->left, result);
		right = eval_expression(expr->right, result);

		if (left->type == right->type)
		{
			result->type = left->type;
			result->value.integer = left->value.integer / right->value.integer;
		}
		return result;

	case EXP_INTEGER:
		result->value.integer = expr->value.integer;
		result->type = expr->type;
		return expr;

	case EXP_REAL:
		result->value.real = expr->value.real;
		result->type = expr->type;
		return expr;

	case EXP_STRING:
		result->value.string = expr->value.string;
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

	if (result.type == EXP_INTEGER)
		printf("%s <- %d\n", assign->ident, result.value.integer);
	else if (result.type == EXP_REAL)
		printf("%s <- %f\n", assign->ident, result.value.real);
	else if (result.type == EXP_STRING)
		printf("%s <- %s\n", assign->ident, result.value.string);


	return 1;
}
