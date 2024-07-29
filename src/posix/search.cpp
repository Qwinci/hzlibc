#include "search.h"
#include "utils.hpp"

EXPORT void* tsearch(const void* key, void** root, int (*comp)(const void* a, const void* b)) {
	__ensure(!"tsearch is not implemented");
}

EXPORT void* tfind(const void* key, const void** root, int (*comp)(const void* a, const void* b)) {
	__ensure(!"tfind is not implemented");
}

EXPORT void* tdelete(
	const void* __restrict key,
	void** __restrict root,
	int (*comp)(const void* a, const void* b)) {
	__ensure(!"tdelete is not implemented");
}

EXPORT void twalk(const void* root, void (*action)(const void* node, VISIT which, int depth)) {
	__ensure(!"twalk is not implemented");
}
