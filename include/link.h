#ifndef _LINK_H
#define _LINK_H

#include <bits/utils.h>
#include <stddef.h>
#include <elf.h>

__begin

#if UINTPTR_MAX == UINT64_MAX
#define ElfW(type) Elf64_ ## type
#else
#define ElfW(type) Elf32_ ## type
#endif

struct dl_phdr_info {
	ElfW(Addr) dlpi_addr;
	const char* dlpi_name;
	const ElfW(Phdr)* dlpi_phdr;
	ElfW(Half) dlpi_phnum;
	unsigned long long dlpi_adds;
	unsigned long long dlpi_subs;
	size_t dlpi_tls_modid;
	void* dlpi_tls_data;
};

struct link_map {
	ElfW(Addr) l_addr;
	char* l_name;
	ElfW(Dyn)* l_ld;
	struct link_map* l_next;
	struct link_map* l_prev;
};

int dl_iterate_phdr(
	int (*__callback)(
		struct dl_phdr_info* __info,
		size_t __size,
		void* __data),
	void* __data);

__end

#endif
