#include "utils.hpp"
#include "stdlib.h"
#include "errno.h"
#include "string.h"

void hzlibc_env_init(char** env);

extern "C" EXPORT int __libc_start_main(
	int (*main)(int, char**, char**),
	int argc,
	char** argv,
	void (*init)(),
	void (*fini)(),
	void (*rtld_fini)(),
	void* stack_end) {
	hzlibc_env_init(argv + argc + 1);
	if (argv[0]) {
		program_invocation_name = argv[0];
		auto ptr = strrchr(program_invocation_name, '/');
		if (ptr) {
			program_invocation_short_name = ptr + 1;
		}
		else {
			program_invocation_short_name = argv[0];
		}
	}
	// todo use rtld_fini when this object exited or unloaded
	auto ret = main(argc, argv, argv + argc + 1);
	exit(ret);
}
