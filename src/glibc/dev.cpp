#include "sys/sysmacros.h"
#include "utils.hpp"

#undef major
#undef minor

EXPORT dev_t gnu_dev_makedev(unsigned int major, unsigned int minor) {
	dev_t dev = static_cast<dev_t>(major & 0xFFF) << 8;
	dev |= static_cast<dev_t>(major & 0xFFFFF000) << 32;
	dev |= static_cast<dev_t>(minor & 0xFF);
	dev |= static_cast<dev_t>(minor & 0xFFFFFF00) << 12;
	return dev;
}

EXPORT unsigned int gnu_dev_major(dev_t dev) {
	return (dev & 0xFFF00) >> 8 | (dev >> 32 & 0xFFFFF000);
}

EXPORT unsigned int gnu_dev_minor(dev_t dev) {
	return (dev & 0xFF) | (dev >> 12 & 0xFFFFFF00);
}
