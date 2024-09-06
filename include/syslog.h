#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <bits/utils.h>
#include <stdarg.h>

__begin_decls

#define LOG_EMERG 0
#define LOG_ALERT 1
#define LOG_CRIT 2
#define LOG_ERR 3
#define LOG_WARNING 4
#define LOG_NOTICE 5
#define LOG_INFO 6
#define LOG_DEBUG 7

#define LOG_PRIMASK 7

#define LOG_KERN (0 << 3)
#define LOG_USER (1 << 3)
#define LOG_MAIL (2 << 3)
#define LOG_DAEMON (3 << 3)
#define LOG_AUTH (4 << 3)
#define LOG_SYSLOG (5 << 3)
#define LOG_LPR (6 << 3)
#define LOG_NEWS (7 << 3)
#define LOG_UUCP (8 << 3)
#define LOG_CRON (9 << 3)
#define LOG_AUTHPRIV (10 << 3)
#define LOG_FTP (11 << 3)
#define LOG_LOCAL0 (16 << 3)
#define LOG_LOCAL1 (17 << 3)
#define LOG_LOCAL2 (18 << 3)
#define LOG_LOCAL3 (19 << 3)
#define LOG_LOCAL4 (20 << 3)
#define LOG_LOCAL5 (21 << 3)
#define LOG_LOCAL6 (22 << 3)
#define LOG_LOCAL7 (23 << 3)

#define LOG_FACMASK 0x3F8

#define LOG_PID 1
#define LOG_CONS 2
#define LOG_ODELAY 4
#define LOG_NDELAY 8
#define LOG_NOWAIT 0x10
#define LOG_PERROR 0x20

void openlog(const char* __ident, int __options, int __facility);
void closelog(void);

void vsyslog(int __priority, const char* __fmt, va_list __ap);
void syslog(int __priority, const char* __fmt, ...);

__end_decls

#endif
