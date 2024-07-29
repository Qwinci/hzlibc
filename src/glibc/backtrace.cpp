#include "execinfo.h"
#include "utils.hpp"
#include "unwind.h"
#include "dlfcn.h"
#include "link.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

namespace {
	struct Info {
		void** buffer;
		uintptr_t cfa;
		int size;
		int count;
	};

	_Unwind_Reason_Code backtrace_helper(_Unwind_Context* ctx, void* arg) {
		auto* info = static_cast<Info*>(arg);
		if (info->count != -1) {
			info->buffer[info->count] = reinterpret_cast<void*>(_Unwind_GetIP(ctx));
			auto cfa = _Unwind_GetCFA(ctx);
			if (info->count > 0 &&
				info->buffer[info->count - 1] == info->buffer[info->count] &&
				info->cfa == cfa) {
				return _URC_END_OF_STACK;
			}
			info->cfa = cfa;
		}

		++info->count;
		if (info->count == info->size) {
			return _URC_END_OF_STACK;
		}

		return _URC_NO_REASON;
	}
}

EXPORT int backtrace(void** buffer, int size) {
	if (size <= 0) {
		return 0;
	}
	Info info {
		.buffer = buffer,
		.cfa {},
		.size = size,
		.count = -1,
	};
	_Unwind_Backtrace(backtrace_helper, &info);
	if (info.count > 1 && !buffer[info.count - 1]) {
		--info.count;
	}
	if (info.count != -1) {
		return info.count;
	}
	else {
		return 0;
	}
}

EXPORT char** backtrace_symbols(const void** buffer, int size) {
	size_t total_size = 0;
	for (int i = 0; i < size; ++i) {
		Dl_info info {};
		if (dladdr(buffer[i], &info)) {
			if (info.dli_fname) {
				total_size += strlen(info.dli_fname);
			}
			if (info.dli_sname) {
				total_size += strlen(info.dli_sname);
			}
			total_size += (sizeof(uintptr_t) * 2 + 2) * 2;
			total_size += 7;
		}
	}

	char** result = static_cast<char**>(malloc(size * sizeof(char*) + total_size));
	if (!result) {
		return nullptr;
	}

	char* ptr = reinterpret_cast<char*>(result + size);
	for (int i = 0; i < size; ++i) {
		result[i] = ptr;

		Dl_info info {};
		link_map* link_map;
		if (dladdr1(buffer[i], &info, reinterpret_cast<void**>(&link_map), RTLD_DL_LINKMAP)) {
			if (info.dli_fname && *info.dli_fname) {
				if (!info.dli_sname) {
					info.dli_saddr = reinterpret_cast<void*>(link_map->l_addr);
				}

				if (info.dli_sname || info.dli_saddr) {
					char sign;
					uintptr_t offset;
					if (buffer[i] >= info.dli_saddr) {
						sign = '+';
						offset = reinterpret_cast<uintptr_t>(buffer[i]) -
							reinterpret_cast<uintptr_t>(info.dli_saddr);
					}
					else {
						sign = '-';
						offset = reinterpret_cast<uintptr_t>(info.dli_saddr) -
							reinterpret_cast<uintptr_t>(buffer[i]);
					}

					ptr += sprintf(
						ptr,
						"%s(%s%c%p) [%p]",
						info.dli_fname,
						info.dli_sname ? info.dli_sname : "",
						sign,
						reinterpret_cast<void*>(offset),
						buffer[i]) + 1;
				}
				else {
					ptr += sprintf(
						ptr,
						"%s [%p]",
						info.dli_fname,
						buffer[i]) + 1;
				}
			}
			else {
				ptr += sprintf(ptr, "[%p]", buffer[i]) + 1;
			}
		}
	}

	return result;
}
