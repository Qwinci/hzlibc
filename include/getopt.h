#ifndef _GETOPT_H
#define _GETOPT_H

#include <bits/utils.h>
#include <unistd.h>

__begin_decls

// glibc

struct option {
	const char* name;
	int has_arg;
	int* flag;
	int val;
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2

int getopt_long(
	int __argc,
	char* const __argv[],
	const char* __opt_str,
	const struct option* __long_opts,
	int* __long_index);
int getopt_long_only(
	int __argc,
	char* const __argv[],
	const char* __opt_str,
	const struct option* __long_opts,
	int* __long_index);

__end_decls

#endif
