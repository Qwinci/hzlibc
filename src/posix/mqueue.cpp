#include "mqueue.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"

EXPORT mqd_t mq_open(const char* name, int oflag, ...) {
	if (*name != '/') {
		errno = EINVAL;
		return -1;
	}

	mode_t mode = 0;
	mq_attr* attr = nullptr;
	if (oflag & O_CREAT) {
		va_list ap;
		va_start(ap, oflag);
		mode = va_arg(ap, mode_t);
		attr = va_arg(ap, mq_attr*);
		va_end(ap);
	}

	++name;

	mqd_t ret;
	if (auto err = sys_mq_open(name, oflag, mode, attr, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int mq_close(mqd_t mq) {
	if (auto err = sys_mq_close(mq)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int mq_unlink(const char* name) {
	if (*name != '/') {
		errno = EINVAL;
		return -1;
	}

	++name;

	if (auto err = sys_mq_unlink(name)) {
		errno = err;
		return -1;
	}
	return 0;
}
