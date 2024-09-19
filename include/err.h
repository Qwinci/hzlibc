#ifndef _ERR_H
#define _ERR_H

#include <bits/utils.h>
#include <stdarg.h>

__begin_decls

void warn(const char* __fmt, ...);
void vwarn(const char* __fmt, va_list __ap);
void warnx(const char* __fmt, ...);
void vwarnx(const char* __fmt, va_list __ap);

__attribute__((__noreturn__)) void err(int __status, const char* __fmt, ...);
__attribute__((__noreturn__)) void verr(int __status, const char* __fmt, va_list __ap);
__attribute__((__noreturn__)) void errx(int __status, const char* __fmt, ...);
__attribute__((__noreturn__)) void verrx(int __status, const char* __fmt, va_list __ap);

__end_decls

#endif
