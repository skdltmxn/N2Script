/*
 *  string.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file contains code related with string operations
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "util.h"
#include "resource.h"

/*
 * Creates a new string
 *
 * Callee is responsible for deleting new string
 */
byte *create_string(const byte *src)
{
    byte *string = NULL;
    size_t len = 0;

    if (!src)
        return NULL;

    len = strlen(src);
    string = (byte *)safe_malloc((len + 1) * sizeof(byte));

    memset(string, 0, len + 1);
    string_copy(string, src, len);

    return string;
}

/*
 * Wrapper for strlen
 */
size_t string_length(const byte *str)
{
    return strlen(str);
}

/*
 * Wrapper for strncpy
 */
byte *string_copy(byte *dest, const byte *src, size_t len)
{
    return strncpy(dest, src, len);
}

/*
 * Concatenate two strings
 *
 * Callee is responsible for deleting new string
 */
byte *string_concat(const byte *left, const byte *right)
{
    byte *string = NULL;
    size_t len = string_length(left) + string_length(right);

    string = (byte *)safe_malloc((len + 1) * sizeof(byte));

    strcpy(string, left);
    strcat(string, right);

    string[len] = '\0';

    return string;
}

byte *string_repeat(const byte *str, const size_t count)
{
    byte *string = NULL;
    size_t len = string_length(str);
    size_t i;

    string = (byte *)safe_malloc((len * count + 1) * sizeof(byte));
    string[len * count] = '\0';

    for (i = 0; i < count; ++i)
        string_copy(string + (i * len), str, len);

    return string;
}

/*
 * Convert hex string into byte
 */
byte string_to_hex(const char *str)
{
    byte hex = 0;

    if (!str || !(str + 1))
        return 0;

    if (str[0] <= '9')
        hex = (str[0] - '0') << 4;
    else if (str[0] <= 'F')
        hex = (str[0] - 'A' + 10) << 4;
    else
        hex = (str[0] - 'a' + 10) << 4;

    if (str[1] <= '9')
        hex |= (str[1] - '0');
    else if (str[1] <= 'F')
        hex |= (str[1] - 'A' + 10);
    else
        hex |= (str[1] - 'a' + 10);

    return hex;
}

/*
 * Encode unicode as UTF-8
 *
 * Supported code points are in range of 0x0000 ~ 0xffff
 * except surrogate pairs (0xd800 ~ 0xdfff)
 */
int encode_utf8(byte *utf8, const ui16 unicode)
{
    /* cannot encode surrogate pairs */
    if (unicode >= 0xd800 && unicode < 0xe000)
        return 0;

    /* ascii */
    if (unicode < 0x80)
    {
        *utf8 = unicode & 0xff;
        return 1;
    }
    /* 2-byte code */
    else if (unicode < 0x800)
    {
        *utf8 = 0xc0 | ((unicode >> 6) & 0x1f);
        *(utf8 + 1) = 0x80 | (unicode & 0x3f);
        return 2;
    }
    /* 3-byte code */
    else
    {
        *utf8 = 0xe0 | ((unicode >> 12) & 0x0f);
        *(utf8 + 1) = 0x80 | ((unicode >> 6) & 0x3f);
        *(utf8 + 2) = 0x80 | (unicode & 0x3f);
        return 3;
    }

    /* impossible */
    return 0;
}

/* Linked list for testing */
struct string_list
{
    struct string_list *next;
    struct rsrc *rsrc;
};

int add_string(void **container, struct rsrc *rsrc)
{
    struct string_list *node = NULL;
    struct string_list **iter = NULL;

    node = (struct string_list *)safe_malloc(sizeof(struct string_list));

    node->next = NULL;
    node->rsrc = rsrc;

    iter = (struct string_list **)container;

    while (*iter) iter = &(*iter)->next;

    *iter = node;

    return 1;
}

const struct rsrc *search_string(void *container, const ui32 id)
{
    struct string_list *node = (struct string_list *)container;

    while (node)
    {
        if (node->rsrc->id == id)
            return node->rsrc;

        node = node->next;
    }

    return NULL;
}

void destroy_rsrc_string(void *container)
{
    struct string_list *node = (struct string_list *)container;
    struct string_list *temp = NULL;

    while (node)
    {
        temp = node->next;
        safe_free(node->rsrc->data);
        safe_free(node->rsrc);
        safe_free(node);
        node = temp;
    }
}