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

extern byte *create_string(const byte *src);
extern size_t string_length(const byte *str);
extern byte *string_copy(byte *dest, const byte *src, size_t len);
extern byte *string_concat(const byte *left, const byte *right);
extern byte *string_repeat(const byte *str, const size_t count);
extern byte string_to_hex(const char *str);
extern int encode_utf8(byte *utf8, const ui16 unicode);

extern ui32 crc32(const void *data, size_t size);

#endif