#ifndef _SEARCH_H
#define _SEARCH_H

#include <bits/utils.h>

__begin_decls

typedef enum {
	preorder,
	postorder,
	endorder,
	leaf
} VISIT;


void* tsearch(const void* __key, void** __root, int (*__comp)(const void* __a, const void* __b));
void* tfind(const void* __key, const void** __root, int (*__comp)(const void* __a, const void* __b));
void* tdelete(
	const void* __restrict __key,
	void** __restrict __root,
	int (*__comp)(const void* __a, const void* __b));
void twalk(const void* __root, void (*__action)(const void* __node, VISIT __which, int __depth));

// glibc
void tdestroy(void* __root, void (*__free_node)(void* __node));

__end_decls

#endif
