#pragma once
#include <stdint.h>

void* __dlapi_open(const char* filename, int flags, uintptr_t return_addr);
int __dlapi_close(void* handle);
void* __dlapi_get_sym(void* __restrict handle, const char* __restrict symbol);
char* __dlapi_get_error();

bool __dlapi_create_tcb(void** tcb, void** tp);
void __dlapi_destroy_tcb(void* tcb);
