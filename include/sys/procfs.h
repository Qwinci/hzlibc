#ifndef _SYS_PROCFS_H
#define _SYS_PROCFS_H

#include <sys/types.h>
#include <sys/user.h>

typedef unsigned long long elf_greg_t;

#define ELF_NGREG ((sizeof(struct user_regs_struct) / sizeof(elf_greg_t)))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct user_fpregs_struct elf_fpregset_t;
typedef elf_gregset_t prgregset_t;
typedef struct user_fpregs_struct prfpregset_t;

#define ELF_PRARGSZ 80

struct elf_siginfo {
	int si_signo;
	int si_code;
	int si_errno;
};

typedef pid_t lwpid_t;
typedef void* psaddr_t;

#endif
