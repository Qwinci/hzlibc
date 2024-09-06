#ifndef _ICONV_H
#define _ICONV_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

typedef void* iconv_t;

iconv_t iconv_open(const char* __to_code, const char* __from_code);
int iconv_close(iconv_t __cd);
size_t iconv(
	iconv_t __cd,
	char** __restrict __in_buf,
	size_t* __restrict __in_bytes_left,
	char** __restrict __out_buf,
	size_t* __restrict __out_bytes_left);

__end_decls

#endif
