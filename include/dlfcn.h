#ifndef _DLFCN_H
#define _DLFCN_H

#include <bits/utils.h>
#include <stdint.h>

__begin_decls

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

// glibc

typedef struct {
	const char* dli_fname;
	void* dli_fbase;
	const char* dli_sname;
	void* dli_saddr;
} Dl_info;

enum {
	RTLD_DL_SYMENT = 1,
	RTLD_DL_LINKMAP = 2
};

#ifdef __i386__
#define DLFO_STRUCT_HAS_EH_DBASE 1
#define DLFO_STRUCT_HAS_EH_COUNT 0
#define DLFO_EH_SEGMENT_TYPE PT_GNU_EH_FRAME
#elif defined(__arm__)
#define DLFO_STRUCT_HAS_EH_DBASE 0
#define DLFO_STRUCT_HAS_EH_COUNT 1
#define DLFO_EH_SEGMENT_TYPE PT_ARM_EXIDX
#else
#define DLFO_STRUCT_HAS_EH_DBASE 0
#define DLFO_STRUCT_HAS_EH_COUNT 0
#define DLFO_EH_SEGMENT_TYPE PT_GNU_EH_FRAME
#endif

struct dl_find_object {
	unsigned long long dlfo_flags;
	void* dlfo_map_start;
	void* dlfo_map_end;
	struct link_map* dlfo_link_map;
	void* dlfo_eh_frame;
#if DLFO_STRUCT_HAS_EH_DBASE
	void* dlfo_eh_dbase;
#if UINTPTR_MAX == UINT32_MAX
	unsigned int __unused0;
#endif
#endif
#if DLFO_STRUCT_HAS_EH_COUNT
	int dlfo_eh_count;
	unsigned int __unused1;
#endif
	unsigned long long __dlfo_unused[7];
};

int dladdr(const void* __addr, Dl_info* __info);
int dladdr1(const void* __addr, Dl_info* __info, void** __extra_info, int __flags);
int dlinfo(void* __restrict __handle, int __request, void* __restrict __info);
int _dl_find_object(void* __addr, struct dl_find_object* __result);

__end_decls

#endif
