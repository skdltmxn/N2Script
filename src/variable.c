/*
 *  variable.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file manages variables used in N2Script
 *
 */

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "util.h"

static ui32 hash(const char *s)
{
    ui32 hash = 0xB40BBE37;
    size_t i;

    for (i = 0; i < strlen(s); ++i)
        hash ^= (hash << 4) ^ s[i];

    return hash;
}

struct var_table *new_var_table(struct var_table *parent)
{
    struct var_table *tbl = NULL;

    tbl = (struct var_table *)safe_malloc(sizeof(*tbl));

    tbl->parent = NULL;
    tbl->next = NULL;
    tbl->child = NULL;

    memset(tbl->entry, 0, sizeof(tbl->entry));

    /* Link parent and child if parent is given */
    if (parent)
    {
        struct var_table **iter = &parent->child;

        while (*iter) iter = &(*iter)->next;

        *iter = tbl;
        tbl->parent = parent;
    }

    return tbl;
}

void destroy_var_entry(struct var_entry *entry)
{
    if (!entry)
        return;

    destroy_var_entry(entry->next);

    safe_free(entry);
}

void destroy_var_table(struct var_table *tbl)
{
    int i;

    if (!tbl)
        return;

    destroy_var_table(tbl->next);
    destroy_var_table(tbl->child);

    for (i = 0; i < MAX_ENTRY; ++i)
        destroy_var_entry(tbl->entry[i]);

    safe_free(tbl);
}

struct expression *resolve_var(const struct expression *expr)
{
    struct var_table *tbl = NULL;
    struct var_entry *entry = NULL;
    ui32 idx = 0;

    idx = hash(expr->value.string) % MAX_ENTRY;

    tbl = expr->vtbl;

    /* Loop from current scope to global */
    while (tbl)
    {
        entry = tbl->entry[idx];
        if (entry)
        {
            /* Look for exact match */
            while (entry && strcmp(entry->name, expr->value.string))
                entry = entry->next;
        }

        tbl = tbl->next;
    }

    if (!entry)
        eval_error("Failed to resolve variable `%s`\n", expr->value.string);

    return &entry->expr;
}

int assign_var(const char *ident, const struct expression *expr)
{
    struct var_table *tbl = NULL;
    struct var_entry *entry = NULL;
    ui32 idx = 0;

    idx = hash(ident) % MAX_ENTRY;

    tbl = expr->vtbl;

    entry = tbl->entry[idx];

    /* If var already exists, update it */
    while (entry)
    {
        if (!strcmp(entry->name, ident))
        {
            memcpy(&entry->expr, expr, sizeof(*expr));
            break;
        }

        entry = entry->next;
    }

    if (!entry)
    {
        struct var_entry **iter = &tbl->entry[idx];

        /* Create new entry */
        entry = (struct var_entry *)safe_malloc(sizeof(*entry));

        entry->name	= ident;
        memcpy(&entry->expr, expr, sizeof(*expr));
        entry->next = NULL;

        while (*iter)
            iter = &(*iter)->next;

        *iter = entry;
    }


    return 1;
}