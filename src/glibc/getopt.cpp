#include "getopt.h"
#include "utils.hpp"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

namespace {
	inline bool is_option(const char* str) {
		return !(*str != '-' || !str[1] || (str[1] == '-' && !str[2]));
	}

	uint32_t next_char = 0;
}

EXPORT int getopt_long(
	int argc,
	char* const argv[],
	const char* opt_str,
	const struct option* long_opts,
	int* long_index) {
	bool stop_on_non_option;
	if (*opt_str == '+') {
		++opt_str;
		stop_on_non_option = true;
	}
	else {
		stop_on_non_option = getenv("POSIXLY_CORRECT");
	}
	bool colon = *opt_str == ':';

	if (!optind) {
		optarg = nullptr;
		optind = 1;
		opterr = 1;
		optopt = 0;
		next_char = 0;
	}

	while (optind < argc) {
		char* arg = argv[optind];

		if (!is_option(arg)) {
			if (stop_on_non_option) {
				return -1;
			}

			int skip_start = optind;
			for (++optind; optind < argc; ++optind) {
				if (is_option(argv[optind])) {
					break;
				}
			}

			if (optind == argc) {
				optind = skip_start;
				return -1;
			}
			continue;
		}

		if (arg[1] == '-') {
			if (!arg[2]) {
				return -1;
			}
			arg += 2;

			auto* name_end = strchr(arg, '=');
			size_t name_len = name_end ? (name_end - arg) : strlen(arg);
			hz::string_view name {arg, name_len};

			for (int i = 0;; ++i) {
				auto& opt = long_opts[i];
				if (!opt.name) {
					break;
				}
				if (name == opt.name) {
					if (opt.has_arg == no_argument) {
						++optind;
						optarg = nullptr;
					}
					else if (opt.has_arg == required_argument) {
						if (name_end) {
							++optind;
							optarg = name_end + 1;
						}
						else if (optind + 1 < argc) {
							optarg = argv[optind + 1];
							optind += 2;
						}
						else {
							if (!colon && opterr) {
								fprintf(stderr, "%s requires an argument\n", arg);
							}

							return colon ? ':' : '?';
						}
					}
					else if (opt.has_arg == optional_argument) {
						if (name_end) {
							++optind;
							optarg = name_end + 1;
						}
						else {
							++optind;
							optarg = nullptr;
						}
					}
					else {
						__ensure(opt.has_arg == no_argument);
						++optind;
						optarg = nullptr;
					}

					if (long_index) {
						*long_index = i;
					}

					if (!opt.flag) {
						return opt.val;
					}
					else {
						*opt.flag = opt.val;
						return 0;
					}
				}
			}

			if (opterr) {
				fprintf(stderr, "--%s is not a valid option\n", arg);
			}
			return '?';
		}
		else {
			++arg;

			auto c = arg[next_char];
			auto pos = strchr(opt_str, c);
			if (pos) {
				if (pos[1] == ':') {
					bool optional = pos[2] == ':';

					if (arg[next_char + 1]) {
						optarg = arg + next_char + 1;
						next_char = 0;
						++optind;
					}
					else if (optind + 1 < argc) {
						if (!optional || !is_option(argv[optind + 1])) {
							optarg = argv[optind + 1];
							next_char = 0;
							optind += 2;
						}
						else {
							optarg = nullptr;
							next_char = 0;
							++optind;
						}
					}
					else {
						if (optional) {
							optarg = nullptr;
							next_char = 0;
							++optind;
						}
						else {
							optopt = static_cast<unsigned char>(arg[next_char]);
							next_char = 0;
							return colon ? ':' : '?';
						}
					}
				}
				else {
					if (arg[next_char + 1]) {
						++next_char;
					}
					else if (arg[next_char]) {
						next_char = 0;
						++optind;
					}
					else {
						return -1;
					}
				}

				return c;
			}
			else {
				if (opterr) {
					fprintf(stderr, "-%c is not a valid option\n", c);
				}
				optopt = static_cast<unsigned char>(c);
				return '?';
			}
		}
	}

	return -1;
}
