#ifndef _MBSTATE_T_H
#define _MBSTATE_T_H

typedef struct {
	int __count;
	union {
		__WINT_TYPE__ __wide;
		char __bytes[4];
	};
} mbstate_t;

#endif
