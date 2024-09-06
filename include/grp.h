#ifndef _GRP_H
#define _GRP_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define NSS_BUFLEN_GROUP 1024

struct group {
	char* gr_name;
	char* gr_passwd;
	gid_t gr_gid;
	char** gr_mem;
};

struct group* getgrnam(const char* __name);
struct group* getgrgid(gid_t __gid);

int getgrnam_r(
	const char* __name,
	struct group* __grp,
	char* __buf,
	size_t __buf_len,
	struct group** __result);
int getgrgid_r(
	gid_t __gid,
	struct group* __grp,
	char* __buf,
	size_t __buf_len,
	struct group** __result);

void setgrent(void);
struct group* getgrent(void);
void endgrent(void);

// bsd
int setgroups(size_t __size, const gid_t* __list);

// glibc
int getgrouplist(const char* __user, gid_t __group, gid_t* __groups, int* __num_groups);

__end_decls

#endif
