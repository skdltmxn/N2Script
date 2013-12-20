/*
 *  resource.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file manages resources used by N2Script internally
 *
 */

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "resource.h"
#include "util.h"

#define RSRC_TYPE_COUNT	1

struct rsrc_pool
{
    void *container;
    int (*insert)(void **, struct rsrc *);
    const struct rsrc * (*search)(void *, const ui32);
    void (*destroy)(void *);
};

/* Each element can point any kind of container */
static struct rsrc_pool rsrc_pool[RSRC_TYPE_COUNT] = {
    { NULL, NULL, NULL, NULL },
};

static int add_rsrc(const enum rsrc_type type, struct rsrc *rsrc)
{
    if (!rsrc_pool[type].insert)
    return 0;

    return rsrc_pool[type].insert(&rsrc_pool[type].container, rsrc);
}

static inline const struct rsrc *search_rsrc(const enum rsrc_type type,
                                             const ui32 id)
{
    return rsrc_pool[type].search(rsrc_pool[type].container, id);
}

void init_rsrc_pool(const enum rsrc_type type,
                    int (*insert)(void **, struct rsrc *),
                    const struct rsrc * (*search)(void *, const ui32),
                    void (*destroy)(void *))
{
    rsrc_pool[type].insert = insert;
    rsrc_pool[type].search = search;
    rsrc_pool[type].destroy = destroy;
}

/*
 * Register string to resource pool
 *
 */
const byte *register_rsrc_string(const byte *s)
{
    /* not sure yet if crc32 is unique enough */
    ui32 id = crc32(s, strlen(s));
    struct rsrc *rsrc = NULL;
    byte *new_string = NULL;

    /* First check if there is existing string */
    if ((rsrc = (struct rsrc *)search_rsrc(RSRC_STRING, id)))
        return (byte *)rsrc->data;

    rsrc = (struct rsrc *)malloc(sizeof(struct rsrc));
    if (!rsrc)
        return NULL;

    new_string = create_string(s);
    rsrc->id = id;
    rsrc->type = RSRC_STRING;
    rsrc->data = (void *)new_string;

    if (!add_rsrc(RSRC_STRING, rsrc))
        return NULL;

    return new_string;
}

void destroy_rsrc()
{
    int i;

    for (i = 0; i < RSRC_TYPE_COUNT; ++i)
        if (rsrc_pool[i].destroy)
            rsrc_pool[i].destroy(rsrc_pool[i].container);
}