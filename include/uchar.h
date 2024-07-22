#ifndef _UCHAR_H
#define _UCHAR_H

#include <bits/utils.h>
#include <bits/mbstate_t.h>
#include <stddef.h>
#include <stdint.h>

__begin

#if !defined(__cplusplus) || __cpp_unicode_characters < 200704L
typedef uint_least32_t char32_t;
#endif

size_t mbrtoc32(
	char32_t* __restrict __pc32,
	const char* __restrict __str,
	size_t __n,
	mbstate_t* __restrict __ps);

__end

#endif
