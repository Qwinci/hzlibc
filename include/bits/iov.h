#ifndef _BITS_IOV_H
#define _BITS_IOV_H

#include <stddef.h>

struct iovec {
	void* iov_base;
	size_t iov_len;
};

#endif
