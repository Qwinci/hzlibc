#ifndef _SYS_UN_H
#define _SYS_UN_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

__end

#endif
