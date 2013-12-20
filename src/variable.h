/*
 * variable.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __VARIABLE_H_
#define __VARIABLE_H_

#define MAX_ENTRY	1024

struct var_entry
{
    struct var_entry *next;
    const char *name;
    struct expression expr;
};

struct var_table
{
    struct var_table *parent, *child, *next;
    struct var_entry *entry[MAX_ENTRY];
};

extern struct var_table *new_var_table(struct var_table *parent);
extern void destroy_var_table(struct var_table *tbl);
extern struct expression *resolve_var(const struct expression *expr);
extern int assign_var(const char *ident, const struct expression *expr);

#endif