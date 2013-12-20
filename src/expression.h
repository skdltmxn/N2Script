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
    EXP_REAL = 0,
    EXP_INTEGER,
    EXP_STRING,
    EXP_IDENT,
    EXP_ADD,
    EXP_SUB,
    EXP_MUL,
    EXP_DIV,
    EXP_XOR,
    EXP_OR,
    EXP_AND
};

union exp_value
{
    const byte *string;
    int integer;
    double real;
};

struct expression
{
    enum expr_type type;
    union exp_value value;
    struct expression *left;
    struct expression *right;
    struct var_table *vtbl;
};

#endif
