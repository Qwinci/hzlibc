#ifndef _SYS_CAPABILITY_H
#define _SYS_CAPABILITY_H

#include <bits/utils.h>
#include <stdint.h>

__begin

typedef struct __user_cap_header_struct {
	uint32_t version;
	int pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
	uint32_t effective;
	uint32_t permitted;
	uint32_t inheritable;
} *cap_user_data_t;

int capget(cap_user_header_t __hdr, cap_user_data_t __data);
int capset(cap_user_header_t __hdr, cap_user_data_t __data);

__end

#endif
