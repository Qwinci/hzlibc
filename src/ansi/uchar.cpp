#include "uchar.h"
#include "utils.hpp"
#include "errno.h"

EXPORT size_t mbrtoc32(
	char32_t* __restrict pc32,
	const char* __restrict str,
	size_t n,
	mbstate_t* __restrict) {
	if (!str) {
		return 0;
	}
	else if (!n) {
		return static_cast<size_t>(-2);
	}

	auto first = static_cast<uint8_t>(*str);
	if (!(first & 1 << 7)) {
		if (pc32) {
			*pc32 = first;
		}
		if (!first) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else if ((first & 0b11100000) == 0b11000000) {
		if (n == 1) {
			return static_cast<size_t>(-2);
		}
		auto second = static_cast<uint8_t>(str[1]);
		if ((second & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (pc32) {
			*pc32 = (second & 0x3F) | (first & 0b11111) << 6;
		}
		return 2;
	}
	else if ((first & 0b11110000) == 0b11100000) {
		if (n == 1) {
			return static_cast<size_t>(-2);
		}
		auto second = static_cast<uint8_t>(str[1]);
		if ((second & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (n == 2) {
			return static_cast<size_t>(-2);
		}
		auto third = static_cast<uint8_t>(str[2]);
		if ((third & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (pc32) {
			*pc32 = (third & 0x3F) | (second & 0x3F) << 6 | (first & 0b1111) << 12;
		}
		return 3;
	}
	else if ((first & 0b11111000) == 0b11110000) {
		if (n == 1) {
			return static_cast<size_t>(-2);
		}
		auto second = static_cast<uint8_t>(str[1]);
		if ((second & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (n == 2) {
			return static_cast<size_t>(-2);
		}
		auto third = static_cast<uint8_t>(str[2]);
		if ((third & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (n == 3) {
			return static_cast<size_t>(-2);
		}
		auto fourth = static_cast<uint8_t>(str[3]);
		if ((fourth & 0b11000000) != 0b10000000) {
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		if (pc32) {
			*pc32 =
				(fourth & 0x3F) |
				(third & 0x3F) << 6 |
				(second & 0x3F) << 12 |
				(first & 0b111) << 18;
		}
		return 4;
	}
	else {
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}
