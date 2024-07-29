#ifndef _BITS_SIGSET_T_H
#define _BITS_SIGSET_T_H

typedef struct {
	unsigned long __value[1024 / (8 * sizeof(unsigned long))];
} sigset_t;

#endif
