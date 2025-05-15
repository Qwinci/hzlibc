#ifndef _SEARCH_H
#define _SEARCH_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

typedef enum {
	preorder,
	postorder,
	endorder,
	leaf
} VISIT;

typedef enum {
	FIND,
	ENTER
} ACTION;

typedef struct entry {
	char* key;
	void* data;
} ENTRY;

void* tsearch(const void* __key, void** __root, int (*__comp)(const void* __a, const void* __b));
void* tfind(const void* __key, void* const* __root, int (*__comp)(const void* __a, const void* __b));
void* tdelete(
	const void* __restrict __key,
	void** __restrict __root,
	int (*__comp)(const void* __a, const void* __b));
void twalk(const void* __root, void (*__action)(const void* __node, VISIT __which, int __depth));

// glibc

struct _ENTRY;

struct hsearch_data {
	struct _ENTRY* table;
	unsigned int size;
	unsigned int filled;
};

int hcreate_r(size_t __num_elems, struct hsearch_data* __htab);
void hdestroy_r(struct hsearch_data* __htab);
int hsearch_r(ENTRY __item, ACTION __action, ENTRY** __ret, struct hsearch_data* __htab);

void tdestroy(void* __root, void (*__free_node)(void* __node));

__end_decls

#endif
