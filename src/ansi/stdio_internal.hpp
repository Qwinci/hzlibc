#pragma once
#include "sys/types.h"
#include "internal/mutex.hpp"

#define FILE_EOF_FLAG 0x10
#define FILE_ERR_FLAG 0x20

struct FILE {
	// glibc
	int flags {};

	// these must equal to null or be used the same way as in glibc
	// because glibc bloats binaries with possibly inline
	// putc_unlocked/getc_unlocked macros that depend on these
	// to know whether to call __overflow/__uflow
	char* read_ptr {};
	char* read_end {};
	char* read_base {};
	char* write_base {};
	char* write_ptr {};
	char* write_end {};

	ssize_t (*write)(FILE* file, const void* data, size_t size) {};
	ssize_t (*read)(FILE* file, void* data, size_t size) {};
	Mutex mutex {};
	int error {};
	int fd {};
	bool last_was_read {};
	bool no_destroy {};
};

FILE* create_fd_file(int fd);
