#include "stdio_unlocked.hpp"
#include "ansi/stdio_internal.hpp"
#include "string.h"
#include "errno.h"

namespace internal {
	size_t fread_unlocked(void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
		if (!size || !count) {
			return 0;
		}
		size_t progress = 0;
		for (; progress < count; ++progress) {
			ssize_t ret = file->read(file, reinterpret_cast<unsigned char*>(buffer) + progress * size, size);
			if (ret < 0 || static_cast<size_t>(ret) < size) {
				return progress;
			}
		}

		return count;
	}

	size_t fwrite_unlocked(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
		if (!size || !count) {
			return 0;
		}
		ssize_t ret = file->write(file, buffer, size * count);
		if (ret < 0) {
			return 0;
		}
		return static_cast<size_t>(ret) / size;
	}

	int fputc_unlocked(int ch, FILE* file) {
		auto c = static_cast<unsigned char>(ch);
		if (file->write(file, &c, sizeof(c)) < 0) {
			return EOF;
		}
		return ch;
	}

	int fputs_unlocked(const char* __restrict str, FILE* __restrict file) {
		if (file->write(file, str, strlen(str)) < 0) {
			return EOF;
		}
		return 0;
	}

	int fgetc_unlocked(FILE* file) {
		unsigned char c;
		ssize_t count_read = file->read(file, &c, 1);
		if (count_read != 1) {
			return EOF;
		}

		return c;
	}

	int getc_unlocked(FILE* file) {
		return internal::fgetc_unlocked(file);
	}

	int getchar_unlocked() {
		return internal::getc_unlocked(stdin);
	}

	char* fgets_unlocked(char* __restrict str, int count, FILE* __restrict file) {
		if (!count) {
			errno = EINVAL;
			return nullptr;
		}

		for (int i = 0;; ++i) {
			if (i == count - 1) {
				str[i] = 0;
				return str;
			}

			int c = internal::fgetc_unlocked(file);
			if (c == EOF) {
				if (i) {
					str[i] = 0;
					return str;
				}
				else {
					return nullptr;
				}
			}
			str[i] = static_cast<char>(c);

			if (c == '\n') {
				str[i + 1] = 0;
				return str;
			}
		}
	}

	int ferror_unlocked(FILE* file) {
		return file->flags & FILE_ERR_FLAG;
	}

	int feof_unlocked(FILE* file) {
		return file->flags & FILE_EOF_FLAG;
	}

	void clearerr_unlocked(FILE* file) {
		file->flags &= ~FILE_ERR_FLAG;
	}

	int fflush_unlocked(FILE* file) {
		return 0;
	}

	int putc_unlocked(int ch, FILE* file) {
		return internal::fputc_unlocked(ch, file);
	}

	int putchar_unlocked(int ch) {
		return internal::fputc_unlocked(ch, stdout);
	}
}
