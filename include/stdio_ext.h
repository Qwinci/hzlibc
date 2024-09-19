#ifndef _STDIO_EXT_H
#define _STDIO_EXT_H

#include <bits/utils.h>
#include <stdio.h>
#include <stddef.h>

__begin_decls

size_t __fpending(FILE* __file);
int __freading(FILE* __file);
int __fwriting(FILE* __file);
void __fpurge(FILE* __file);
size_t __freadahead(FILE* __file);
void __fseterr(FILE* __file);
const char* __freadptr(FILE* __file, size_t* __size);
void __freadptrinc(FILE* __file, size_t __increment);

__end_decls

#endif
