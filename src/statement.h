/*
 *  statement.h
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __STATEMENT_H_
#define __STATEMENT_H_

#include "expression.h"

struct assign_stmt
{
	const char *ident;
	struct expression *expr;
};

struct statement
{
	struct statement *next;

	struct assign_stmt *assign;
	int (*execute)(const struct statement *stmt);
	void (*destroy)(struct statement *stmt);
};

#endif
