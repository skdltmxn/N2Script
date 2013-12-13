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

/*
 * Creates an unicode string
 */
byte *create_string(byte *src)
{
	byte *string = NULL;
	size_t len = 0;

	if (!src)
		return NULL;

	len = strlen(src);
	string = (byte *)malloc((len + 1) * sizeof(byte));
	if (!string)
		return NULL;

	memset(string, 0, len + 1);
	string_copy(string, src, len);

	return string;
}

/*
 * Wrapper for strlen
 */
size_t string_length(byte *str)
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

byte *string_concat(byte *left, byte *right)
{
	byte *string = NULL;
	size_t len = string_length(left) + string_length(right);

	string = (byte *)malloc((len + 1) * sizeof(byte));

	if (!string)
		return NULL;

	strcpy(string, left);
	strcat(string, right);

	string[len] = '\0';

	return string;
}