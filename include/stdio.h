#ifndef _STDIO_H
#define _STDIO_H

#include <bits/utils.h>
#include <sys/types.h>
#include <stdarg.h>

__begin

typedef struct FILE FILE;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EOF -1

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define BUFSZ 8192

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#define stdin stdin
#define stdout stdout
#define stderr stderr

int fputc(int __ch, FILE* __file);
int putc(int __ch, FILE* __file);
int putchar(int __ch);

int fputs(const char* __restrict __str, FILE* __restrict __file);
int puts(const char* __str);

void perror(const char* __str);

__attribute__((__format__(printf, 2, 0))) int vfprintf(FILE* __restrict __file, const char* __restrict __fmt, va_list __ap);
__attribute__((__format__(printf, 2, 3))) int fprintf(FILE* __restrict __file, const char* __restrict __fmt, ...);
__attribute__((__format__(printf, 1, 0))) int vprintf(const char* __restrict __fmt, va_list __ap);
__attribute__((__format__(printf, 1, 2))) int printf(const char* __restrict __fmt, ...);

__attribute__((__format__(printf, 3, 0))) int vsnprintf(
	char* __restrict __buffer,
	size_t __size,
	const char* __restrict __fmt,
	va_list __ap);
__attribute__((__format__(printf, 3, 4))) int snprintf(
	char* __restrict __buffer,
	size_t __size,
	const char* __restrict __fmt,
	...);
__attribute__((__format__(printf, 2, 3))) int sprintf(char* __restrict __buffer, const char* __restrict __fmt, ...);

FILE* fopen(const char* __restrict __filename, const char* __restrict __mode);
size_t fread(void* __restrict __buffer, size_t __size, size_t __count, FILE* __file);
size_t fwrite(const void* __restrict __buffer, size_t __size, size_t __count, FILE* __restrict __file);
int fgetc(FILE* __file);
char* fgets(char* __restrict __str, int __count, FILE* __restrict __file);
int fclose(FILE* __file);
int ferror(FILE* __file);
int feof(FILE* __file);

int fseek(FILE* __file, long __offset, int __origin);
void rewind(FILE* __file);

int setvbuf([[maybe_unused]] FILE* __file, char* __restrict __buffer, int __mode, size_t __size);
int fflush(FILE* __file);

// posix
FILE* fdopen(int __fd, const char* __mode);
int fileno(FILE* __file);
int fseeko(FILE* __file, off_t __offset, int __whence);

ssize_t getline(char** __restrict __line, size_t* __restrict __size, FILE* __restrict __file);

// glibc
size_t fread_unlocked(void* __restrict __buffer, size_t __size, size_t __count, FILE* __restrict __file);
size_t fwrite_unlocked(const void* __restrict __buffer, size_t __size, size_t __count, FILE* __restrict __file);
int fputc_unlocked(int __ch, FILE* __file);
int fputs_unlocked(const char* __restrict __str, FILE* __restrict __file);
int fgetc_unlocked(FILE* __file);
int ferror_unlocked(FILE* __file);
int feof_unlocked(FILE* __file);
void clearerr_unlocked(FILE* __file);
int fflush_unlocked(FILE* __file);
int putc_unlocked(int __ch, FILE* __file);
int putchar_unlocked(int __ch);

int vasprintf(char** __restrict __ptr, const char* __fmt, va_list __ap);
int asprintf(char** __restrict __ptr, const char* __fmt, ...);

__end

#endif
