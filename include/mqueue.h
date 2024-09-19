#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <bits/utils.h>

__begin_decls

typedef int mqd_t;

struct mq_attr {
	long mq_flags;
	long mq_maxmsg;
	long mq_msgsize;
	long mq_curmsgs;
	long __unused[4];
};

mqd_t mq_open(const char* __name, int __oflag, ...);
int mq_close(mqd_t __mq);
int mq_unlink(const char* __name);

__end_decls

#endif
