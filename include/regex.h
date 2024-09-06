#ifndef _REGEX_H
#define _REGEX_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

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

__end_decls

#endif
