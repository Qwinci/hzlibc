#ifndef _MALLOC_H
#define _MALLOC_H

#include <bits/utils.h>
#include <stddef.h>

__begin

struct mallinfo2 {
	size_t arena;
	size_t ordblks;
	size_t smblks;
	size_t hblks;
	size_t hblkhd;
	size_t usmblks;
	size_t fsmblks;
	size_t uordblks;
	size_t fordblks;
	size_t keepcost;
};

int malloc_trim(size_t __pad);
struct mallinfo2 mallinfo2(void);

__end

#endif
