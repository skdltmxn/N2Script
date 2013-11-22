/*
 * types.h
 *
 *   Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 * This file defines common types
 *
 */

#ifndef __TYPES_H_
#define __TYPES_H_

#include <wchar.h>

/* Windows */
#ifdef _WIN32
	#pragma warning(disable:4996)
	#ifdef _WIN64
		#define x64
	#else
		#define x86
	#endif
#endif

typedef unsigned char   byte;
typedef unsigned short  ui16;
typedef unsigned int    ui32;
typedef unsigned long long ui64;
typedef short           i16;
typedef int             i32;
typedef long long       i64;

#ifndef NULL
#define NULL	0
#endif

#endif
