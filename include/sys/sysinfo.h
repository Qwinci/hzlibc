#ifndef _SYS_SYSINFO_H
#define _SYS_SYSINFO_H

#include <bits/utils.h>
#include <stdint.h>

__begin_decls

#define SI_LOAD_SHIFT 16

struct sysinfo {
	long uptime;
	unsigned long loads[3];
	unsigned long totalram;
	unsigned long freeram;
	unsigned long sharedram;
	unsigned long bufferram;
	unsigned long totalswap;
	unsigned long freeswap;
	uint16_t procs;
	uint16_t pad;
	unsigned long totalhigh;
	unsigned long freehigh;
	uint32_t mem_unit;
	char __pad[20 - 2 * sizeof(unsigned long) - sizeof(uint32_t)];
};

// glibc
int get_nprocs(void);

__end_decls

#endif
