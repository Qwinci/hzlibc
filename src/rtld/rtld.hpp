#pragma once
#include <stdint.h>
#include "link.h"

struct DlInfo {
	const char* object_path;
	uintptr_t object_base;
	const char* symbol_name;
	uintptr_t symbol_address;
	const ElfW(Sym)* symbol;
	link_map* map;
};

void* __dlapi_open(const char* filename, int flags, uintptr_t return_addr);
int __dlapi_close(void* handle);
void* __dlapi_get_sym(void* __restrict handle, const char* __restrict symbol);
int __dlapi_dladdr(uintptr_t addr, DlInfo* info);
int __dlapi_dlinfo(void* __restrict handle, int request, void* __restrict info);
int __dlapi_iterate_phdr(
	int (*callback)(
		dl_phdr_info* info,
		size_t size,
		void* data),
	void* data);
char* __dlapi_get_error();

bool __dlapi_create_tcb(void** tcb, void** tp);
void __dlapi_destroy_tcb(void* tcb);
