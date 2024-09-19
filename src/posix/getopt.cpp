#include "unistd.h"
#include "utils.hpp"
#include "string.h"
#include "stdio.h"

EXPORT char* optarg = nullptr;
EXPORT int optind = 1;
EXPORT int opterr = 1;
EXPORT int optopt = 0;

static char* CURRENT_PTR = nullptr;

EXPORT int getopt(int argc, char* const argv[], const char* opt_string) {
	optarg = nullptr;

	if (!CURRENT_PTR || !*CURRENT_PTR) {
		if (optind == 0) {
			++optind;
		}

		if (optind >= argc || *argv[optind] != '-' || argv[optind][1] == 0) {
			return -1;
		}
		else if (argv[optind][1] == '-' && argv[optind][2] == 0) {
			++optind;
			return -1;
		}

		CURRENT_PTR = argv[optind] + 1;
		++optind;
	}

	char c = *CURRENT_PTR++;
	char* contained = strchr(opt_string, c);
	if (!contained || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return '?';
	}

	++contained;
	if (*contained == ':') {
		if (*CURRENT_PTR) {
			optarg = CURRENT_PTR;
			CURRENT_PTR = nullptr;
		}
		else if (optind < argc) {
			optarg = argv[optind];
			++optind;
		}
		else {
			fprintf(stderr, "%s: option required argument -%c\n", argv[0], c);
			return ':';
		}
	}

	return c;
}
