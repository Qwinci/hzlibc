#ifndef _STDIO_EXT_H
#define _STDIO_EXT_H

#include <bits/utils.h>
#include <stdio.h>
#include <stddef.h>

__begin

size_t __fpending(FILE* __file);
int __freading(FILE* __file);
void __fpurge([[maybe_unused]] FILE* __file);

__end

#endif
