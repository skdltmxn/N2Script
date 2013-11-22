/*
 * expression.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __EXPRESSION_H_
#define __EXPRESSION_H_

enum expr_type
{
	EXP_FLOAT = 0,
	EXP_NUM,
	EXP_STRING,
	EXP_IDENT,
	EXP_ADD,
	EXP_SUB,
	EXP_MUL,
	EXP_DIV
};


struct expression
{
	int type;
	void *value;
	struct expression *left;
	struct expression *right;
};

#endif
