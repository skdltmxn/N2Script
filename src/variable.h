/*
 * variable.h
 *
 *   Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __VARIABLE_H_
#define __VARIABLE_H_

#define MAX_ENTRY	1024

#include "node.h"

struct var_entry
{
    struct var_entry *next;
    const char *name;
    node n;
};

struct var_table
{
    struct var_table *parent, *child, *next;
    struct var_entry *entry[MAX_ENTRY];
};

extern void init_var_table();
extern void destroy_var_table();
extern const node *resolve_var(const node *n);
extern int assign_var(const char *ident, const node *n);

#endif