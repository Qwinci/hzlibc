#ifndef _RESOLV_H
#define _RESOLV_H

#include <bits/utils.h>

__begin_decls

typedef struct __res_state* res_state;

int res_search(
	const char* __domain_name,
	int __class,
	int __type,
	unsigned char* __answer,
	int __answer_len);
int res_query(
	const char* __domain_name,
	int __class,
	int __type,
	unsigned char* __answer,
	int __answer_len);

int res_ninit(res_state __state);
void res_nclose(res_state __state);
int res_nquery(
	res_state __state,
	const char* __domain_name,
	int __class,
	int __type,
	unsigned char* __answer,
	int __answer_len);
int dn_expand(
	const unsigned char* __msg,
	const unsigned char* __eom_orig,
	const unsigned char* __comp_dn,
	char* __expanded_dn,
	int __length);

__end_decls

#endif
