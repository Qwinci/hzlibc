#ifndef _MALLOC_H
#define _MALLOC_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

struct mallinfo {
	int arena;
	int ordblks;
	int smblks;
	int hblks;
	int hblkhd;
	int usmblks;
	int fsmblks;
	int uordblks;
	int fordblks;
	int keepcost;
};

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
struct mallinfo mallinfo(void);
struct mallinfo2 mallinfo2(void);
size_t malloc_usable_size(void* __ptr);
void* memalign(size_t __alignment, size_t __size);

__end_decls

#endif
