#ifndef _GRP_H
#define _GRP_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

#define NSS_BUFLEN_GROUP 1024

struct group {
	char* gr_name;
	char* gr_passwd;
	gid_t gr_gid;
	char** gr_mem;
};

struct group* getgrnam(const char* __name);
struct group* getgrgid(gid_t __gid);

void setgrent(void);
struct group* getgrent(void);
void endgrent(void);

__end

#endif
