#ifndef _SYS_USER_H
#define _SYS_USER_H

#include <stdint.h>

#ifdef __x86_64__

typedef struct user_fpregs_struct {
	uint16_t cwd;
	uint16_t swd;
	uint16_t ftw;
	uint16_t fop;
	uint64_t rip;
	uint64_t rdp;
	uint32_t mxcsr;
	uint32_t mxcsr_mask;
	uint32_t st_space[32];
	uint32_t xmm_space[64];
	uint32_t padding[24];
} elf_fpregset_t;

struct user_regs_struct {
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t rbp;
	uint64_t rbx;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rax;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t orig_rax;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t fs_base;
	uint64_t gs_base;
	uint64_t ds;
	uint64_t es;
	uint64_t fs;
	uint64_t gs;
};

struct user {
	struct user_regs_struct regs;
	int u_fpvalid;
	struct user_fpregs_struct i387;
	uint64_t u_tsize;
	uint64_t u_dsize;
	uint64_t u_ssize;
	uint64_t start_code;
	uint64_t start_stack;
	int64_t signal;
	int reserved;
	struct user_regs_struct* u_ar0;
	struct user_fpregs_struct* u_fpstate;
	uint64_t magic;
	char u_comm[32];
	uint64_t u_debugreg[8];
};

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#else

#error missing architecture specific code

#endif

#endif
