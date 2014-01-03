/*
 * resource.h
 *
 *   Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __RESOURCE_H_
#define __RESOURCE_H_

enum rsrc_type
{
    RSRC_STRING = 0
};

struct rsrc
{
    ui32 id;
    enum rsrc_type	type;
    void *data;
};

extern void init_rsrc_pool(const enum rsrc_type type,
                           int (*insert)(void **, struct rsrc *),
                           const struct rsrc * (*search)(void *, const ui32),
                           void (*destroy)(void *));
extern const byte *register_rsrc_string(const byte *s);
extern void destroy_rsrc();

#endif