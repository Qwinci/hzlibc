#include "dlfcn.h"
#include "utils.hpp"
#include "rtld/rtld.hpp"

EXPORT int dladdr(const void* addr, Dl_info* info) {
	DlInfo dl_info {};
	if (!__dlapi_dladdr(reinterpret_cast<uintptr_t>(addr), &dl_info)) {
		return 0;
	}
	info->dli_fname = dl_info.object_path;
	info->dli_fbase = reinterpret_cast<void*>(dl_info.object_base);
	info->dli_sname = dl_info.symbol_name;
	info->dli_saddr = reinterpret_cast<void*>(dl_info.symbol_address);
	return 1;
}

EXPORT int dladdr1(const void* addr, Dl_info* info, void** extra_info, int flags) {
	DlInfo dl_info {};
	if (!__dlapi_dladdr(reinterpret_cast<uintptr_t>(addr), &dl_info)) {
		return 0;
	}
	info->dli_fname = dl_info.object_path;
	info->dli_fbase = reinterpret_cast<void*>(dl_info.object_base);
	info->dli_sname = dl_info.symbol_name;
	info->dli_saddr = reinterpret_cast<void*>(dl_info.symbol_address);

	if (flags == RTLD_DL_LINKMAP) {
		*extra_info = dl_info.map;
	}
	else if (flags == RTLD_DL_SYMENT) {
		*extra_info = const_cast<ElfW(Sym)*>(dl_info.symbol);
	}

	return 1;
}

EXPORT int dlinfo(void* __restrict handle, int request, void* __restrict info) {
	return __dlapi_dlinfo(handle, request, info);
}
