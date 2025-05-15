#ifndef _SYS_PARAM_H
#define _SYS_PARAM_H

#include <endian.h>
#include <limits.h>

#define NOFILE 256

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#if !defined(MAXPATHLEN)
#define MAXPATHLEN PATH_MAX
#endif

#endif
