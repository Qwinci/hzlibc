#include "string.hpp"
#include "wchar.h"
#include <hz/string_view.hpp>

namespace internal {
	char* strtok_r(char* __restrict str, const char* __restrict delim, char** __restrict save_ptr) {
		if (str) {
			*save_ptr = str;
		}

		hz::string_view str_view {*save_ptr};
		hz::string_view delim_str {delim};

		auto* start = *save_ptr;

		auto start_pos = str_view.find_first_not_of(delim_str);
		if (start_pos == hz::string_view::npos) {
			return nullptr;
		}

		auto end_pos = str_view.find_first_of(delim_str, start_pos);
		if (end_pos != hz::string_view::npos) {
			(*save_ptr)[end_pos] = 0;
			*save_ptr += end_pos + 1;
		}
		else {
			*save_ptr += end_pos;
		}
		return start + start_pos;
	}

	size_t mbsnrtowcs(
		wchar_t* __restrict dest,
		const char** __restrict src,
		size_t num_chars,
		size_t len,
		mbstate_t* __restrict ps) {
		size_t written = 0;
		for (size_t i = 0; i < num_chars;) {
			wchar_t buf;
			size_t bytes_used = mbrtowc(&buf, *src, num_chars - i, ps);
			if (bytes_used == static_cast<size_t>(-1)) {
				return static_cast<size_t>(-1);
			}
			else if (bytes_used == static_cast<size_t>(-2)) {
				break;
			}

			if (dest) {
				if (written + 1 > len) {
					return written;
				}
				else {
					*dest++ = buf;
				}
			}

			if (buf == 0) {
				*src = nullptr;
				break;
			}
			else {
				*src += bytes_used;
				i += bytes_used;
			}

			++written;
		}

		return written;
	}
}
