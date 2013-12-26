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

struct if_stmt
{
    struct expression *condition;
    struct statement *true_stmts;
    struct statement *false_stmts;
};

struct statement
{
    struct statement *next;

    union
    {
        struct assign_stmt *assign;
        struct if_stmt *ifstmt;
    };

    int (*execute)(const struct statement *stmt);
    void (*destroy)(struct statement *stmt);
};

#endif
