#ifndef _SYS_MTIO_H
#define _SYS_MTIO_H

#include <sys/ioctl.h>

struct mtop {
	short mt_op;
	int mt_count;
};

#define MTREW 6

#define MTIOCTOP _IOR('m', 1, struct mtop)

#endif
