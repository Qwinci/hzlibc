#include "search.h"
#include "utils.hpp"

EXPORT int hcreate_r(size_t num_elems, struct hsearch_data* __htab) {
	__ensure(!"hcreate_r is not implemented");
}

EXPORT void hdestroy_r(struct hsearch_data* __htab) {
	__ensure(!"hdestroy_r is not implemented");
}

EXPORT int hsearch_r(ENTRY __item, ACTION __action, ENTRY** __ret, struct hsearch_data* __htab) {
	__ensure(!"hsearch_r is not implemented");
}

EXPORT void tdestroy(void* root, void (*free_node)(void* node)) {
	//__ensure(!"tdestroy is not implemented");
	//println("tdestroy is not implemented");
}
