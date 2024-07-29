#include "libgen.h"
#include "utils.hpp"
#include "string.h"

EXPORT char* basename(char* path) {
	if (!path || !*path) {
		return const_cast<char*>(".");
	}

	if (auto ptr = strrchr(path, '/')) {
		if (ptr[1]) {
			return ptr + 1;
		}
		else {
			for (; ptr > path && ptr[-1] == '/';) --ptr;
			if (ptr > path) {
				*ptr-- = 0;
				for (; ptr > path && ptr[-1] != '/';) --ptr;
			}
			else {
				while (ptr[1]) {
					++ptr;
				}
			}

			return ptr;
		}
	}
	else {
		return path;
	}
}
