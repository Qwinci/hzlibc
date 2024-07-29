#ifndef _ARPA_NAMESER_H
#define _ARPA_NAMESER_H

#include <bits/utils.h>
#include <stdint.h>

__begin

#define NS_PACKETSZ 512
#define NS_MAXDNAME 1025

typedef enum __ns_sect {
	ns_s_qd = 0,
	ns_s_zn = 0,
	ns_s_an = 1,
	ns_s_pr = 1,
	ns_s_ns = 2,
	ns_s_ud = 2,
	ns_s_ar = 3,
	ns_s_max = 4
} ns_sect;

typedef struct __ns_msg {
	const unsigned char* _msg;
	const unsigned char* _eom;
	uint16_t _id;
	uint16_t _flags;
	uint16_t _counts[ns_s_max];
	const unsigned char* _sections[ns_s_max];
	ns_sect _sect;
	int _rrnum;
	const unsigned char* _msg_ptr;
} ns_msg;

typedef struct __ns_rr {
	char name[NS_MAXDNAME];
	uint16_t type;
	uint16_t rr_class;
	uint32_t ttl;
	uint16_t rdlength;
	const unsigned char* rdata;
} ns_rr;

int ns_initparse(const unsigned char* __msg, int __msg_len, ns_msg* __handle);
int ns_parserr(ns_msg* __handle, ns_sect __section, int __rr_num, ns_rr* __rr);
unsigned int ns_get16(const unsigned char* __ptr);

__end

#endif
