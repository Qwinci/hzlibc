#include "stdio_ext.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"

EXPORT size_t __fpending(FILE*) {
	return 0;
}

EXPORT int __freading(FILE* file) {
	auto lock = file->mutex.lock();
	return file->last_was_read;
}

EXPORT int __fwriting(FILE* file) {
	auto lock = file->mutex.lock();
	return !file->last_was_read;
}

EXPORT void __fpurge(FILE* file) {

}

EXPORT size_t __freadahead(FILE* file) {
	auto lock = file->mutex.lock();
	return file->ungetc_size;
}

EXPORT void __fseterr(FILE* file) {
	auto lock = file->mutex.lock();
	file->flags |= FILE_ERR_FLAG;
}

EXPORT const char* __freadptr(FILE* file, size_t* size) {
	auto lock = file->mutex.lock();
	if (file->ungetc_size == 0) {
		return nullptr;
	}
	*size = file->ungetc_size;
	return reinterpret_cast<const char*>(file->ungetc_ptr);
}

EXPORT void __freadptrinc(FILE* file, size_t increment) {
	auto lock = file->mutex.lock();
	__ensure(increment <= static_cast<size_t>(file->ungetc_size));

	if (increment == static_cast<size_t>(file->ungetc_size)) {
		file->ungetc_ptr = file->ungetc_buffer;
		file->ungetc_size = 0;
	}
	else {
		while (increment--) {
			--file->ungetc_ptr;
			--file->ungetc_size;
		}
	}
}
