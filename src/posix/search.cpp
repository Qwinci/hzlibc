#include "search.h"
#include "utils.hpp"

EXPORT void* tsearch(const void* key, void** root, int (*comp)(const void* a, const void* b)) {
	// todo
	//println("tsearch is not implemented");
	return nullptr;
}

EXPORT void* tfind(const void* key, void* const* root, int (*comp)(const void* a, const void* b)) {
	if (!root) {
		return nullptr;
	}

	// todo
	//println("tfind is not implemented");
	return nullptr;
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
