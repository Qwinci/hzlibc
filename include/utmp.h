#ifndef _UTMP_H
#define _UTMP_H

#include <bits/utils.h>
#include <sys/types.h>
#include <sys/time.h>

__begin_decls

#define UT_LINESIZE 32
#define UT_NAMESIZE 32
#define UT_HOSTSIZE 256

#define BOOT_TIME 2

struct exit_status {
	short e_termination;
	short e_exit;
};

struct utmp {
	short ut_type;
	pid_t ut_pid;
	char ut_line[UT_LINESIZE];
	char ut_id[4];
	char ut_user[UT_NAMESIZE];
	char ut_host[UT_HOSTSIZE];
	struct exit_status ut_exit;
	int32_t ut_session;
#if UINTPTR_MAX == UINT64_MAX
	struct {
		int32_t tv_sec;
		int32_t tv_usec;
	} ut_tv;
#else
	struct timeval ut_tv;
#endif
	int32_t ut_addr_v6[4];
	char __unused[20];
};

#define ut_name ut_user
#ifndef _NO_UT_TIME
#define ut_time ut_tv.tv_sec
#endif

__end_decls

#endif
