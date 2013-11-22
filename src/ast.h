/*
 * ast.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __AST_T_
#define __AST_T_

#include "statement.h"

struct block
{
	struct statement *stmts;
};

struct ast_tree
{
	struct block *blk;
};

extern struct ast_tree *init_ast();
extern void destroy_ast(struct ast_tree *root);
extern struct block *new_block();
extern struct statement *new_statement(int (*execute)(struct statement *stmt),
		void (*destroy)(struct statement *stmt));
extern struct assign_stmt *new_assign_stmt(char *token, struct expression *expr);
extern void destroy_assign_stmt(struct statement *stmt);
extern void add_statement(struct block *blk, struct statement *stmt);
extern struct expression *new_expression(const enum expr_type type, void *value);
extern struct expression *new_operation(const enum expr_type type,
		struct expression *left, struct expression *right);

#endif
