#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#include <sys/types.h>
#include <bits/utils.h>

__begin_decls

dev_t gnu_dev_makedev(unsigned int __major, unsigned int __minor);
unsigned int gnu_dev_major(dev_t __dev);
unsigned int gnu_dev_minor(dev_t __dev);

#define makedev gnu_dev_makedev
#define major gnu_dev_major
#define minor gnu_dev_minor

__end_decls

#endif
