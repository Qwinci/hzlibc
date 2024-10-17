#pragma once
#include "stdio.h"

namespace internal {
	size_t fread_unlocked(void* __restrict buffer, size_t size, size_t count, FILE* __restrict file);
	size_t fwrite_unlocked(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file);
	int fputc_unlocked(int ch, FILE* file);
	int fputs_unlocked(const char* __restrict str, FILE* __restrict file);
	int fgetc_unlocked(FILE* file);
	int getc_unlocked(FILE* file);
	int getchar_unlocked();
	char* fgets_unlocked(char* __restrict str, int count, FILE* __restrict file);
	int ferror_unlocked(FILE* file);
	int feof_unlocked(FILE* file);
	void clearerr_unlocked(FILE* file);
	int fflush_unlocked(FILE* file);
	int putc_unlocked(int ch, FILE* file);
	int putchar_unlocked(int ch);
}
