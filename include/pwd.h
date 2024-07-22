#ifndef _PWD_H
#define _PWD_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

struct passwd {
	char* pw_name;
	char* pw_passwd;
	uid_t pw_uid;
	gid_t pw_gid;
	char* pw_gecos;
	char* pw_dir;
	char* pw_shell;
};

#define NSS_BUFLEN_PASSWD 1024

struct passwd* getpwnam(const char* __name);
struct passwd* getpwuid(uid_t __uid);

void setpwent(void);
struct passwd* getpwent(void);
void endpwent(void);

// glibc
int getpwuid_r(
	uid_t __uid,
	struct passwd* __restrict __pwd,
	char* __restrict buffer,
	size_t __buffer_size,
	struct passwd** __restrict __result);

__end

#endif
