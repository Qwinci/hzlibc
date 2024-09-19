#ifndef _SYS_KLOG_H
#define _SYS_KLOG_H

#include <bits/utils.h>

__begin_decls

int klogctl(int __type, char* __buf, int __len);

__end_decls

#endif
