/*
 * ast.c
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 * This file contains code for construction/destruction of the AST tree
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "ast.h"

struct ast_tree *init_ast()
{
	struct ast_tree *root = NULL;

	root = (struct ast_tree *)malloc(sizeof(*root));

	if (root)
		root->blk = NULL;

	return root;
}

void destroy_expression(struct expression *expr)
{
	if (!expr)
		return;

	destroy_expression(expr->left);
	destroy_expression(expr->right);

	if (expr->type == EXP_STRING || expr->type == EXP_IDENT)
		free(expr->value);

	free(expr);
}

void destroy_statement(struct statement *stmt)
{
	if (!stmt)
		return;

	destroy_statement(stmt->next);

	stmt->destroy(stmt);
	free(stmt);
}

void destroy_block(struct block *blk)
{
	if (!blk)
		return;

	destroy_statement(blk->stmts);

	free(blk);
}

void destroy_ast(struct ast_tree *root)
{
	if (!root)
		return;

	destroy_block(root->blk);

	free(root);
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
	free(stmt->assign->ident);
	free(stmt->assign);
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

struct expression *new_expression(const enum expr_type type, void *value)
{
	struct expression *expr = NULL;

	expr = (struct expression *)malloc(sizeof(*expr));

	if (expr)
	{
		expr->left = NULL;
		expr->right = NULL;
		expr->type = type;
		expr->value = value;
	}

	return expr;
}

struct expression *new_operation(const enum expr_type type,
		struct expression *left, struct expression *right)
{
	struct expression *expr = NULL;

	expr = (struct expression *)malloc(sizeof(*expr));

	if (expr)
	{
		expr->left = left;
		expr->right = right;
		expr->type = type;
		expr->value = NULL;
	}

	return expr;
}


