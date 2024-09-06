#include <stdint.h>
#include <hz/bit.hpp>

static uint64_t udivdi3(uint64_t numerator, uint64_t denominator, uint64_t* rem_ptr) {
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

	*rem_ptr = rem;
	return quot;
}

extern "C" uint64_t __udivdi3(uint64_t numerator, uint64_t denominator) {
	uint64_t rem;
	return udivdi3(numerator, denominator, &rem);
}

extern "C" uint64_t __umoddi3(uint64_t numerator, uint64_t denominator) {
	uint64_t rem;
	udivdi3(numerator, denominator, &rem);
	return rem;
}

extern "C" int64_t __divdi3(int64_t numerator, int64_t denominator) {
	uint64_t abs_numerator = numerator < 0 ? numerator * -1 : numerator;
	uint64_t abs_denominator = denominator < 0 ? denominator * -1 : denominator;

	uint64_t rem;
	uint64_t quot = udivdi3(abs_numerator, abs_denominator, &rem);

	bool negate = (numerator ^ denominator) < 0;
	if (negate) {
		return static_cast<int64_t>(quot ^ static_cast<int64_t>(-1)) + 1;
	}
	else {
		return static_cast<int64_t>(quot);
	}
}

extern "C" int64_t __moddi3(int64_t numerator, int64_t denominator) {
	uint64_t abs_numerator = numerator < 0 ? numerator * -1 : numerator;
	uint64_t abs_denominator = denominator < 0 ? denominator * -1 : denominator;

	uint64_t rem;
	udivdi3(abs_numerator, abs_denominator, &rem);

	if (numerator < 0) {
		return static_cast<int64_t>(rem ^ static_cast<int64_t>(-1)) + 1;
	}
	else {
		return static_cast<int64_t>(rem);
	}
}

extern "C" [[noreturn]] void __stack_chk_fail();

extern "C" [[noreturn]] void __stack_chk_fail_local() {
	__stack_chk_fail();
}
