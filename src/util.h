/*
 * util.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __UTIL_H_
#define __UTIL_H_

#define safe_free(p) \
	if ((p)) { \
		free((p)); \
		(p) = NULL; \
	}

extern byte *create_string(byte *src);
extern size_t string_length(byte *str);
extern byte *string_copy(byte *dest, const byte *src, size_t len);
extern byte *string_concat(byte *left, byte *right);

#endif