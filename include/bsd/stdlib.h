#ifndef _BSD_STDLIB_H
#define _BSD_STDLIB_H

#include <bits/utils.h>
#include <stddef.h>
#include <stdint.h>

__begin_decls

uint32_t arc4random(void);
void arc4random_buf(void* __buf, size_t __num_bytes);

int getloadavg(double __load_avg[], int __num_elem);

__end_decls

#endif
