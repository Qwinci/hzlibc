#ifndef _UTMPX_H
#define _UTMPX_H

#include <bits/utils.h>
#include <sys/types.h>
#include <sys/time.h>

__begin_decls

struct __exit_status {
	short e_termination;
	short e_exit;
};

struct utmpx {
	short ut_type;
	pid_t ut_pid;
	char ut_line[32];
	char ut_id[4];
	char ut_user[32];
	char ut_host[256];
	struct __exit_status ut_exit;
	int32_t ut_session;
	struct {
		uint32_t tv_sec;
		int32_t tv_usec;
	} ut_tv;
	int32_t ut_addr_v6[4];
};

struct utmpx* getutxent(void);
void setutxent(void);
void endutxent(void);

__end_decls

#endif
