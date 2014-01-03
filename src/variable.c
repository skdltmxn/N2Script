/*
 *  variable.c
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 *  This file manages variables used in N2Script
 *
 */

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "variable.h"
#include "eval.h"
#include "util.h"

static struct var_table *global = NULL;
static struct var_table *current = NULL;

static ui32 hash(const char *s)
{
    ui32 hash = 0xB40BBE37;
    size_t i;

    for (i = 0; i < strlen(s); ++i)
        hash ^= (hash << 4) ^ s[i];

    return hash;
}

static struct var_table *new_var_table(struct var_table *parent)
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

static void destroy_var_entry(struct var_entry *entry)
{
    if (!entry)
        return;

    destroy_var_entry(entry->next);

    safe_free(entry);
}

static void delete_var_table(struct var_table *tbl)
{
    int i;

    if (!tbl)
        return;

    delete_var_table(tbl->next);
    delete_var_table(tbl->child);

    for (i = 0; i < MAX_ENTRY; ++i)
        destroy_var_entry(tbl->entry[i]);

    safe_free(tbl);
}

void init_var_table()
{
    struct var_table *tbl = new_var_table(NULL);

    global = current = tbl;
}

void destroy_var_table()
{
    delete_var_table(global);
}

const node *resolve_var(const node *n)
{
    struct var_table *tbl = NULL;
    struct var_entry *entry = NULL;
    ui32 idx = 0;

    idx = hash(n->val.sval) % MAX_ENTRY;

    tbl = current;

    /* Loop from current scope to global */
    while (tbl)
    {
        entry = tbl->entry[idx];
        if (entry)
        {
            /* Look for exact match */
            while (entry && strcmp(entry->name, n->val.sval))
                entry = entry->next;
        }

        tbl = tbl->next;
    }

    if (!entry)
        eval_error("Failed to resolve variable `%s`\n", n->val.sval);

    return &entry->n;
}

int assign_var(const char *ident, const node *n)
{
    struct var_table *tbl = NULL;
    struct var_entry *entry = NULL;
    ui32 idx = 0;

    idx = hash(ident) % MAX_ENTRY;

    tbl = current;

    entry = tbl->entry[idx];

    /* If var already exists, update it */
    while (entry)
    {
        if (!strcmp(entry->name, ident))
        {
            memcpy(&entry->n, n, sizeof(*n));
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
        memcpy(&entry->n, n, sizeof(*n));
        entry->next = NULL;

        while (*iter)
            iter = &(*iter)->next;

        *iter = entry;
    }


    return 1;
}