#include "setjmp.h"
#include "utils.hpp"
#include "signal.h"

extern "C" [[gnu::returns_twice]] int __sigsetjmp(jmp_buf env, int save_mask) {
	env->__saved_mask = save_mask && sigprocmask(SIG_BLOCK, nullptr, &env->__sigset) == 0;
	return 0;
}

EXPORT [[gnu::noreturn]] void siglongjmp(sigjmp_buf env, int value) {
	if (env->__saved_mask) {
		sigprocmask(SIG_SETMASK, &env->__sigset, nullptr);
	}
	_longjmp(env, value);
}
