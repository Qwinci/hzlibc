#include "fnmatch.h"
#include "utils.hpp"
#include "ctype.h"

EXPORT int fnmatch(const char* pattern, const char* str, int flags) {
	bool path = flags & FNM_PATHNAME;
	bool fold = flags & FNM_CASEFOLD;

	bool in_star = false;
	const char* pattern_at_star;
	while (true) {
		loop:
		char c = *pattern;
		if (in_star) {
			if (c != 0) {
				++pattern;
			}
		}
		else {
			if (c == 0) {
				if (*str) {
					if ((flags & FNM_LEADING_DIR) && *str == '/') {
						return 0;
					}
					else {
						return FNM_NOMATCH;
					}
				}
				else {
					break;
				}
			}
			++pattern;
		}

		char str_c = *str;
		if (!str_c) {
			if (in_star || c == '*') {
				return 0;
			}

			return FNM_NOMATCH;
		}

		++str;
		if (fold) {
			// todo use casefold
			c = static_cast<char>(tolower(c));
			str_c = static_cast<char>(tolower(c));
		}

		if (c == '?') {
			if (path && str_c == '/') {
				if (in_star) {
					pattern = pattern_at_star;
					continue;
				}
				else {
					if ((flags & FNM_LEADING_DIR) && !*pattern) {
						return 0;
					}
					else {
						return FNM_NOMATCH;
					}
				}
			}

			in_star = false;
		}
		else if (c == '*') {
			in_star = true;
			pattern_at_star = pattern;
			--str;
		}
		else if (c == '[') {
			bool exclude;
			bool found = false;
			if (*pattern == '!' || *pattern == '^') {
				exclude = true;
				found = true;
				++pattern;
			}
			else {
				exclude = false;
			}

			auto* group_start = pattern;
			while (*pattern && (*pattern != ']' || pattern == group_start)) {
				if (pattern[0] == '[' && pattern[1] == ':') {
					auto start = pattern + 2;
					while (pattern[0] != ':' || pattern[1] != ']') {
						if (!*pattern) {
							return FNM_NOMATCH;
						}
						++pattern;
					}

					// this is invalid
					if (pattern == start) {
						return FNM_NOMATCH;
					}

					hz::string_view name {start, static_cast<size_t>(pattern - start)};

					bool match;

					if (name == "alnum") {
						match = isalnum(str_c);
					}
					else if (name == "alpha") {
						match = isalpha(str_c);
					}
					else if (name == "blank") {
						match = isblank(str_c);
					}
					else if (name == "cntrl") {
						match = iscntrl(str_c);
					}
					else if (name == "digit") {
						match = isdigit(str_c);
					}
					else if (name == "graph") {
						match = isgraph(str_c);
					}
					else if (name == "lower") {
						match = islower(str_c);
					}
					else if (name == "print") {
						match = isprint(str_c);
					}
					else if (name == "punct") {
						match = ispunct(str_c);
					}
					else if (name == "space") {
						match = isspace(str_c);
					}
					else if (name == "upper") {
						match = isupper(str_c);
					}
					else if (name == "xdigit") {
						match = isxdigit(str_c);
					}
					else {
						match = false;
					}

					if (match) {
						if (exclude) {
							if (in_star) {
								pattern = pattern_at_star;
								goto loop;
							}
							else {
								return FNM_NOMATCH;
							}
						}
						else {
							found = true;
						}
					}

					pattern += 2;
					continue;
				}

				char current_c = *pattern;
				if (fold) {
					current_c = static_cast<char>(tolower(current_c));
				}

				if (pattern[1] == '-' && pattern[2]) {
					auto start = current_c;

					char end = pattern[2];
					if (fold) {
						end = static_cast<char>(tolower(end));
					}

					if (str_c >= start && str_c <= end) {
						if (exclude) {
							if (in_star) {
								pattern = pattern_at_star;
								goto loop;
							}
							else {
								return FNM_NOMATCH;
							}
						}

						found = true;
					}

					pattern += 3;
					continue;
				}

				if (exclude) {
					if (current_c == str_c && (!path || str_c != '/')) {
						if (in_star) {
							pattern = pattern_at_star;
							goto loop;
						}
						else {
							return FNM_NOMATCH;
						}
					}
				}
				else {
					if (current_c == str_c && (!path || str_c != '/')) {
						found = true;
					}
				}

				++pattern;
			}

			// missing ]
			if (!*pattern) {
				return FNM_NOMATCH;
			}

			if (!found) {
				if (in_star) {
					pattern = pattern_at_star;
					continue;
				}
				else {
					return FNM_NOMATCH;
				}
			}

			in_star = false;
			++pattern;
		}
		else {
			if (str_c != c) {
				if (in_star && (!path || str_c != '/')) {
					pattern = pattern_at_star;
					continue;
				}
				else {
					if ((flags & FNM_LEADING_DIR) && !*pattern && str_c == '/') {
						return 0;
					}
					else {
						return FNM_NOMATCH;
					}
				}
			}

			in_star = false;
		}
	}

	return 0;
}
