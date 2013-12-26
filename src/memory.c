/*
 *  crc.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file defines safe_malloc
 *
 */

#include <stdlib.h>
#include "types.h"

/*
 * Never failing malloc
 */
void *safe_malloc(size_t size)
{
    void *memory = NULL;

    do
    {
        memory = malloc(size);
    } while (memory == NULL);

    return memory;
}