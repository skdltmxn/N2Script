/*
 *  ast.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
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

struct ast_tree *init_ast()
{
	struct ast_tree *root = NULL;

	root = (struct ast_tree *)malloc(sizeof(*root));

	if (root)
	{
		root->blk = NULL;
		root->scanner = NULL;
	}

	return root;
}

void destroy_expression(struct expression *expr)
{
	if (!expr)
		return;

	destroy_expression(expr->left);
	destroy_expression(expr->right);

	if (expr->type == EXP_STRING || expr->type == EXP_IDENT)
		if (expr->value.string)
			safe_free(expr->value.string);

	safe_free(expr);
}

void destroy_statement(struct statement *stmt)
{
	if (!stmt)
		return;

	destroy_statement(stmt->next);

	stmt->destroy(stmt);
	safe_free(stmt);
}

void destroy_block(struct block *blk)
{
	if (!blk)
		return;

	destroy_statement(blk->stmts);

	safe_free(blk);
}

void destroy_ast(struct ast_tree *root)
{
	if (!root)
		return;

	destroy_block(root->blk);

	safe_free(root);
}

struct block *new_block()
{
	struct block *blk = NULL;

	blk = (struct block *)malloc(sizeof(*blk));

	if (blk)
		blk->stmts = NULL;

	return blk;
}

struct statement *new_statement(int (*execute)(struct statement *stmt),
		void (*destroy)(struct statement *stmt))
{
	struct statement *stmt = NULL;

	stmt = (struct statement *)malloc(sizeof(*stmt));

	if (stmt)
	{
		stmt->next = NULL;
		stmt->execute = execute;
		stmt->destroy = destroy;
	}

	return stmt;
}

struct assign_stmt *new_assign_stmt(char *token, struct expression *expr)
{
	struct assign_stmt *stmt = NULL;

	if (!token || !expr)
		return NULL;

	stmt = (struct assign_stmt *)malloc(sizeof(*stmt));

	if (stmt)
	{
		stmt->ident = token;
		stmt->expr = expr;
	}

	return stmt;
}

void destroy_assign_stmt(struct statement *stmt)
{
	destroy_expression(stmt->assign->expr);
	safe_free(stmt->assign->ident);
	safe_free(stmt->assign);
}

void add_statement(struct block *blk, struct statement *stmt)
{
	struct statement **iter = NULL;
	if (!blk || !stmt)
		return;

	iter = &blk->stmts;
	while (*iter) iter = &(*iter)->next;

	*iter = stmt;
}

struct expression *new_expression(const enum expr_type type,
		union exp_value *value)
{
	struct expression *expr = NULL;

	expr = (struct expression *)malloc(sizeof(*expr));

	if (expr)
	{
		expr->left = NULL;
		expr->right = NULL;
		expr->type = type;
		memcpy(&expr->value, value, sizeof(*value));
	}

	return expr;
}

struct expression *new_operation(const enum expr_type type,
		struct expression *left, struct expression *right)
{
	struct expression *expr = NULL;
	int check = TYPE_CHECK_OK;

	expr = (struct expression *)malloc(sizeof(*expr));

	if (!expr)
		return NULL;

	check = type_check(type, left, right);

	/* cannot evaluate for now */
	if (check == TYPE_CHECK_PENDING)
	{
		expr->left = left;
		expr->right = right;
		expr->type = type;
		return expr;
	}
	/* syntax error */
	else if (check == TYPE_CHECK_ERROR)
		parse_error(get_ast_root(), "type error");


	/* evaluate as much as possible */
	expr->left = NULL;
	expr->right = NULL;

	if (type == EXP_ADD)
	{
		if (add_expression(left, right, expr))
		{
			destroy_expression(left);
			destroy_expression(right);
			return expr;
		}
	}
	else if (type == EXP_SUB)
	{
		if (sub_expression(left, right, expr))
		{
			destroy_expression(left);
			destroy_expression(right);
			return expr;
		}
	}
	else if (type == EXP_MUL)
	{
		if (mul_expression(left, right, expr))
		{
			destroy_expression(left);
			destroy_expression(right);
			return expr;
		}
	}
	else if (type == EXP_DIV)
	{
		if (div_expression(left, right, expr))
		{
			destroy_expression(left);
			destroy_expression(right);
			return expr;
		}
	}

	return expr;
}

void parse_error(struct ast_tree *root, const char *msg)
{
	YYLTYPE *loc = yyget_lloc(root->scanner);
	fprintf(stderr, "%s (line: %d, col: %d)", msg, loc->first_line, loc->first_column);
	yylex_destroy(root->scanner);
	destroy_all();
	exit(-3);
}
