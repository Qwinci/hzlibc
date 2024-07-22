#ifndef _DLFCN_H
#define _DLFCN_H

#include <bits/utils.h>

__begin

#define RTLD_NEXT ((void*) -1)
#define RTLD_DEFAULT ((void*) 0)

#define RTLD_LOCAL 0
#define RTLD_LAZY 0x1
#define RTLD_NOW 0x2
#define RTLD_NOLOAD 0x4
#define RTLD_DEEPBIND 0x8
#define RTLD_GLOBAL 0x100
#define RTLD_NODELETE 0x1000

void* dlopen(const char* __filename, int __flags);
int dlclose(void* __handle);
void* dlsym(void* __restrict __handle, const char* __restrict __symbol);
char* dlerror(void);

__end

#endif
