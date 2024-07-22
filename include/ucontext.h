#ifndef _UCONTEXT_H
#define _UCONTEXT_H

#include <bits/utils.h>
#include <signal.h>

__begin

typedef struct {
	void* ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;

#ifdef __x86_64__

#define NREG 23

typedef int64_t greg_t;
typedef greg_t gregset_t[NREG];

#define REG_R8 0
#define REG_R9 1
#define REG_R10 2
#define REG_R11 3
#define REG_R12 4
#define REG_R13 5
#define REG_R14 6
#define REG_R15 7
#define REG_RDI 8
#define REG_RSI 9
#define REG_RBP 10
#define REG_RBX 11
#define REG_RDX 12
#define REG_RAX 13
#define REG_RCX 14
#define REG_RSP 15
#define REG_RIP 16
#define REG_EFL 17
#define REG_CSGSFS 18
#define REG_ERR 19
#define REG_TRAPNO 20
#define REG_OLDMASK 21
#define REG_CR2 22

struct __fpreg {
	uint16_t significand[4];
	uint16_t exponent;
	uint16_t __reserved[3];
};

struct __fpxmmreg {
	uint32_t element[4];
};

struct __fpstate {
	uint16_t cwd;
	uint16_t swd;
	uint16_t ftw;
	uint16_t fop;
	uint64_t rip;
	uint64_t rdp;
	uint32_t mxcsr;
	uint32_t mxcsr_mask;
	__fpreg _st[8];
	__fpxmmreg _xmm[16];
	uint32_t __reserved0[12];
	uint32_t __reserved1[12];
};

typedef struct __fpstate* fpregset_t;

typedef struct {
	gregset_t gregs;
	fpregset_t fpregs;
	uint64_t __reserved[8];
} mcontext_t;

typedef struct ucontext_t {
	unsigned long uc_flags;
	struct ucontext_t* uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	sigset_t uc_sigmask;
} ucontext_t;

#elif defined(__i386__)

#define NREG 19

typedef int32_t greg_t;
typedef greg_t gregset_t[NREG];

#define REG_GS 0
#define REG_FS 1
#define REG_ES 2
#define REG_DS 3
#define REG_EDI 4
#define REG_ESI 5
#define REG_EBP 6
#define REG_ESP 7
#define REG_EBX 8
#define REG_EDX 9
#define REG_ECX 10
#define REG_EAX 11
#define REG_TRAPNO 12
#define REG_ERR 13
#define REG_EIP 14
#define REG_CS 15
#define REG_EFL 16
#define REG_UESP 17
#define REG_SS 18

struct __fpreg {
	uint16_t significand[4];
	uint16_t exponent;
	uint16_t __reserved[3];
};

struct __fpstate {
	uint32_t cw;
	uint32_t sw;
	uint32_t tag;
	uint32_t ipoff;
	uint32_t cssel;
	uint32_t dataoff;
	uint32_t datasel;
	__fpreg _st[8];
	uint32_t status;
};

typedef struct __fpstate* fpregset_t;

typedef struct {
	gregset_t gregs;
	fpregset_t fpregs;
	uint32_t oldmask;
	uint32_t cr2;
} mcontext_t;

typedef struct ucontext_t {
	unsigned long uc_flags;
	struct ucontext_t* uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	sigset_t uc_sigmask;
} ucontext_t;

#else

#error missing architecture specific code

#endif

int setcontext(const ucontext_t* __ctx);

__end

#endif
