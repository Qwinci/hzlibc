#include "sys/ipc.h"
#include "utils.hpp"
#include "sys/stat.h"

EXPORT key_t ftok(const char* path, int proj_id) {
	struct stat s {};
	if (stat(path, &s) < 0) {
		return -1;
	}
	return static_cast<key_t>((s.st_ino & 0xFFFF) | ((s.st_dev & 0xFF) << 16) | ((proj_id & 0xFF) << 24));
}
