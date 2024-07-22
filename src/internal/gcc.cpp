#include <stdint.h>
#include <hz/bit.hpp>

extern "C" uint64_t __udivdi3(uint64_t numerator, uint64_t denominator) {
	if (denominator == 0) {
		volatile int dump = 0;
		dump /= dump;
		(void) dump;
	}

	uint64_t quot = 0;
	uint64_t rem = 0;
	auto bits = hz::bit_width(numerator);
	for (int i = bits; i > 0; --i) {
		rem <<= 1;
		rem |= (numerator >> (i - 1) & 1);
		if (rem >= denominator) {
			rem -= denominator;
			quot |= 1ULL << (i - 1);
		}
	}
	return quot;
}

extern "C" uint64_t __umoddi3(uint64_t numerator, uint64_t denominator) {
	if (denominator == 0) {
		volatile int dump = 0;
		dump /= dump;
		(void) dump;
	}

	uint64_t rem = 0;
	auto bits = hz::bit_width(numerator);
	for (int i = bits; i > 0; --i) {
		rem <<= 1;
		rem |= (numerator >> (i - 1) & 1);
		if (rem >= denominator) {
			rem -= denominator;
		}
	}
	return rem;
}
