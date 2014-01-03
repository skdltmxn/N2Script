/*
 * string.h
 *
 *   Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __STRING_H_
#define __STRING_H_

extern int add_string(void **container, struct rsrc *rsrc);
extern const struct rsrc *search_string(void *container, const ui32 id);
extern void destroy_rsrc_string(void *container);

#endif