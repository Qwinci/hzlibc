#ifndef _REGEX_H
#define _REGEX_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

#define REG_ENOSYS -1
#define REG_NOERROR 0
#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13
#define REG_EEND 14
#define REG_ESIZE 15
#define REG_ERPAREN 16

#define REG_EXTENDED (1 << 0)
#define REG_ICASE (1 << 1)
#define REG_NEWLINE (1 << 2)
#define REG_NOSUB (1 << 3)

typedef struct re_pattern_buffer {
	struct re_dfa_t* __buffer;
	unsigned long __allocated;
	unsigned long __used;
	unsigned long __syntax;
	char* __fastmap;
	unsigned char* __translate;
	size_t re_nsub;
	unsigned int __can_be_null : 1;
	unsigned int __regs_allocated : 2;
	unsigned int __fastmap_accurate : 1;
	unsigned int __no_sub : 1;
	unsigned int __not_bol : 1;
	unsigned int __not_eol : 1;
	unsigned int __newline_anchor : 1;
} regex_t;

typedef int regoff_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

int regcomp(regex_t* __reg, const char* __regex, int __flags);
void regfree(regex_t* __reg);
int regexec(
	const regex_t* __restrict __reg,
	const char* __restrict __str,
	size_t __num_match,
	regmatch_t* __restrict __match,
	int __flags);
size_t regerror(
	int __err_code,
	const regex_t* __regex,
	char* __err_buf,
	size_t __err_buf_size);

__end_decls

#endif
