#ifndef _INTTYPES_H
#define _INTTYPES_H

#include <bits/utils.h>
#include <stdint.h>

__begin

uintmax_t strtoumax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
intmax_t strtoimax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);

__end

#endif
