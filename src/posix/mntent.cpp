#include "mntent.h"
#include "utils.hpp"

EXPORT FILE* setmntent(const char* file_name, const char* type) {
	__ensure(!"setmntent is not implemented");
}

EXPORT mntent* getmntent(FILE* file) {
	__ensure(!"getmntent is not implemented");
}

EXPORT int endmntent(FILE* file) {
	__ensure(!"endmntent is not implemented");
}

EXPORT char* hasmntopt(const mntent* __restrict mnt, const char* __restrict option) {
	__ensure(!"hasmntopt is not implemented");
}

// this is glibc but to avoid making the state global its here
EXPORT mntent* getmntent_r(
	FILE* __restrict file,
	struct mntent* __restrict mnt_buf,
	char* __restrict buf,
	int buf_len) {
	__ensure(!"getmntent_r is not implemented");
}
