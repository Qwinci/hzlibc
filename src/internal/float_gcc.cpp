#include <stdint.h>
#include <hz/bit.hpp>
#include <fenv.h>
#include <limits.h>

#if __LDBL_MANT_DIG__ == 113 || (__FLT_RADIX__ == 16 && __LDBL_MANT_DIG__ == 28)
using tf_float = long double;
#define TF_C(x) x ## L
#elif defined(__FLOAT128__) || defined(__SIZEOF_FLOAT128__)
typedef __float128 tf_float;
#define TF_C(x) x ## Q
#endif

namespace {
	namespace flt128 {
		using rep_t = __uint128_t;
		using srep_t = __int128_t;
		using half_rep_t = uint64_t;

		constexpr rep_t toRep(tf_float x) {
			return hz::bit_cast<rep_t>(x);
		}

		constexpr tf_float fromRep(rep_t x) {
			return hz::bit_cast<tf_float>(x);
		}

		inline int rep_clz(rep_t a) {
			struct S {
				uint64_t low;
				uint64_t high;
			};
			auto uu = hz::bit_cast<S>(a);

			uint64_t word;
			uint64_t add;

			if (uu.high) {
				word = uu.high;
				add = 0;
			}
			else {
				word = uu.low;
				add = 64;
			}
			return static_cast<int>(__builtin_clzll(word) + add);
		}

		constexpr auto typeWidth = sizeof(rep_t) * 8;
		constexpr auto significandBits = 112;
		constexpr auto exponentBits = typeWidth - significandBits - 1;
		constexpr auto maxExponent = (1 << exponentBits) - 1;
		constexpr auto exponentBias = maxExponent >> 1;
		constexpr auto implicitBit = rep_t {1} << significandBits;
		constexpr auto significandMask = implicitBit - 1U;
		constexpr auto signBit = rep_t {1} << (significandBits + exponentBits);
		constexpr auto absMask = signBit - 1U;
		constexpr auto exponentMask = absMask ^ significandMask;
		constexpr auto infRep = exponentMask;
		constexpr auto quietBit = implicitBit >> 1;
		constexpr auto qnanRep = exponentMask | quietBit;

		inline int normalize(rep_t* significand) {
			const int shift = rep_clz(*significand) - rep_clz(implicitBit);
			*significand <<= shift;
			return 1 - shift;
		}

		inline void wideMultiply(rep_t a, rep_t b, rep_t* hi, rep_t* lo) {
			const uint64_t product = (uint64_t) a * b;
			*hi = (rep_t) (product >> 32);
			*lo = (rep_t) product;
		}

		inline void wideLeftShift(rep_t* hi, rep_t* lo, unsigned int count) {
			*hi = *hi << count | *lo >> (typeWidth - count);
			*lo = *lo << count;
		}

		inline void wideRightShiftWithSticky(rep_t* hi, rep_t* lo, unsigned int count) {
			if (count < typeWidth) {
				const bool sticky = (*lo << (typeWidth - count)) != 0;
				*lo = *hi << (typeWidth - count) | *lo >> count | sticky;
				*hi = *hi >> count;
			} else if (count < 2 * typeWidth) {
				const bool sticky = *hi << (2 * typeWidth - count) | *lo;
				*lo = *hi >> (count - typeWidth) | sticky;
				*hi = 0;
			} else {
				const bool sticky = *hi | *lo;
				*lo = sticky;
				*hi = 0;
			}
		}
	}
}

template<typename T>
struct FltStats;

template<>
struct FltStats<float> {
	static constexpr int ExpBits = 8;
	static constexpr int SigFracBits = 23;
	using rep_t = uint32_t;
};

template<>
struct FltStats<double> {
	static constexpr int ExpBits = 11;
	static constexpr int SigFracBits = 52;
	using rep_t = uint64_t;
};

template<>
struct FltStats<tf_float> {
	static constexpr int ExpBits = 15;
	static constexpr int SigFracBits = 112;
	using rep_t = __uint128_t;
};

#if UINT_MAX == 0xFFFFFFFF
#define clzsi __builtin_clz
#define ctzsi __builtin_ctz
#elif ULONG_MAX == 0xFFFFFFFF
#define clzsi __builtin_clzl
#define ctzsi __builtin_ctzl
#else
#error could not determine appropriate clzsi macro for this system
#endif

template<typename Src>
static inline tf_float __extendXfYf2__(Src x) {
	constexpr int srcExpBits = FltStats<Src>::ExpBits;
	constexpr int dstExpBits = FltStats<tf_float>::ExpBits;
	constexpr int srcBits = sizeof(Src) * 8;
	constexpr int dstBits = sizeof(tf_float) * 8;
	constexpr int srcSigFracBits = FltStats<Src>::SigFracBits;
	constexpr int dstSigFracBits = FltStats<tf_float>::SigFracBits;
	using src_rep_t = FltStats<Src>::rep_t;
	using dst_rep_t = FltStats<tf_float>::rep_t;

	auto srcToRep = [](Src x) {
		return hz::bit_cast<src_rep_t>(x);
	};

	auto dstFromRep = [](dst_rep_t x) {
		return hz::bit_cast<tf_float>(x);
	};

	auto extract_sign_from_src = [](src_rep_t x) {
		const src_rep_t srcSignMask = src_rep_t {1} << (srcBits - 1);
		return (x & srcSignMask) >> (srcBits - 1);
	};

	auto extract_exp_from_src = [](src_rep_t x) {
		const int srcSigBits = srcBits - 1 - srcExpBits;
		const src_rep_t srcExpMask = ((src_rep_t {1} << srcExpBits) - 1) << srcSigBits;
		return (x & srcExpMask) >> srcSigBits;
	};

	auto extract_sig_frac_from_src = [](src_rep_t x) {
		const src_rep_t srcSigFracMask = (src_rep_t {1} << srcSigFracBits) - 1;
		return x & srcSigFracMask;
	};

	auto clz_in_sig_frac = [](src_rep_t sigFrac) {
		const int skip = 1 + srcExpBits;

		if constexpr (hz::is_same_v<Src, double>) {
			return __builtin_clzll(sigFrac) - skip;
		}
		else if constexpr (hz::is_same_v<Src, float>) {
			return ctzsi(sigFrac) - skip;
		}
		else {
			static_assert(false);
		}
	};

	auto construct_dst_rep = [](dst_rep_t sign, dst_rep_t exp, dst_rep_t sigFrac) {
		return (sign << (dstBits - 1)) | (exp << (dstBits - 1 - dstExpBits)) | sigFrac;
	};

	// Various constants whose values follow from the type parameters.
	// Any reasonable optimizer will fold and propagate all of these.
	const int srcInfExp = (1 << srcExpBits) - 1;
	const int srcExpBias = srcInfExp >> 1;

	const int dstInfExp = (1 << dstExpBits) - 1;
	const int dstExpBias = dstInfExp >> 1;

	// Break x into a sign and representation of the absolute value.
	const src_rep_t aRep = srcToRep(x);
	const src_rep_t srcSign = extract_sign_from_src(aRep);
	const src_rep_t srcExp = extract_exp_from_src(aRep);
	const src_rep_t srcSigFrac = extract_sig_frac_from_src(aRep);

	dst_rep_t dstSign = srcSign;
	dst_rep_t dstExp;
	dst_rep_t dstSigFrac;

	if (srcExp >= 1 && srcExp < (src_rep_t)srcInfExp) {
		// x is a normal number.
		dstExp = (dst_rep_t)srcExp + (dst_rep_t)(dstExpBias - srcExpBias);
		dstSigFrac = (dst_rep_t)srcSigFrac << (dstSigFracBits - srcSigFracBits);
	}
	else if (srcExp == srcInfExp) {
		// x is NaN or infinity.
		dstExp = dstInfExp;
		dstSigFrac = (dst_rep_t)srcSigFrac << (dstSigFracBits - srcSigFracBits);
	}
	else if (srcSigFrac) {
		// a is denormal.
		if (srcExpBits == dstExpBits) {
			// The exponent fields are identical and this is a denormal number, so all
			// the non-significand bits are zero. In particular, this branch is always
			// taken when we extend a denormal F80 to F128.
			dstExp = 0;
			dstSigFrac = ((dst_rep_t)srcSigFrac) << (dstSigFracBits - srcSigFracBits);
		}
		else {
			// Renormalize the significand and clear the leading bit.
			// For F80 -> F128 this codepath is unused.
			const int scale = clz_in_sig_frac(srcSigFrac) + 1;
			dstExp = dstExpBias - srcExpBias - scale + 1;
			dstSigFrac = (dst_rep_t)srcSigFrac
				<< (dstSigFracBits - srcSigFracBits + scale);
			const dst_rep_t dstMinNormal = dst_rep_t {1} << (dstBits - 1 - dstExpBits);
			dstSigFrac ^= dstMinNormal;
		}
	}
	else {
		// x is zero.
		dstExp = 0;
		dstSigFrac = 0;
	}

	const dst_rep_t result = construct_dst_rep(dstSign, dstExp, dstSigFrac);
	return dstFromRep(result);
}

extern "C" tf_float __extenddftf2(double x) {
	return __extendXfYf2__<double>(x);
}

#ifdef __aarch64__
using CMP_RESULT = int;
#elif __SIZEOF_POINTER__ == 8 && __SIZEOF_LONG__ == 4
using CMP_RESULT = long long;
#else
using CMP_RESULT = long;
#endif

enum {
	LE_LESS = -1,
	LE_EQUAL = 0,
	LE_GREATER = 1,
	LE_UNORDERED = 1,
};

static inline CMP_RESULT __leXf2__(tf_float a, tf_float b) {
	using namespace flt128;

	const auto aInt = static_cast<srep_t>(toRep(a));
	const auto bInt = static_cast<srep_t>(toRep(b));
	const rep_t aAbs = aInt & absMask;
	const rep_t bAbs = bInt & absMask;

	// If either a or b is NaN, they are unordered.
	if (aAbs > infRep || bAbs > infRep)
		return LE_UNORDERED;

	// If a and b are both zeros, they are equal.
	if ((aAbs | bAbs) == 0)
		return LE_EQUAL;

	// If at least one of a and b is positive, we get the same result comparing
	// a and b as signed integers as we would with a floating-point compare.
	if ((aInt & bInt) >= 0) {
		if (aInt < bInt)
			return LE_LESS;
		else if (aInt == bInt)
			return LE_EQUAL;
		else
			return LE_GREATER;
	}
	else {
		// Otherwise, both are negative, so we need to flip the sense of the
		// comparison to get the correct result.  (This assumes a twos- or ones-
		// complement integer representation; if integers are represented in a
		// sign-magnitude representation, then this flip is incorrect).
		if (aInt > bInt)
			return LE_LESS;
		else if (aInt == bInt)
			return LE_EQUAL;
		else
			return LE_GREATER;
	}
}

extern "C" CMP_RESULT __letf2(tf_float a, tf_float b) {
	return __leXf2__(a, b);
}

extern "C" [[gnu::alias("__letf2")]] typeof(__letf2) __cmptf2;
extern "C" [[gnu::alias("__letf2")]] typeof(__letf2) __eqtf2;
extern "C" [[gnu::alias("__letf2")]] typeof(__letf2) __lttf2;
extern "C" [[gnu::alias("__letf2")]] typeof(__letf2) __netf2;

using fixint_t = int64_t;
using fixuint_t = uint64_t;

static inline fixint_t __fixint(tf_float x) {
	using namespace flt128;

	const auto fixint_max = (fixint_t) ((~(fixuint_t) 0) / 2);
	const fixint_t fixint_min = -fixint_max - 1;
	// Break x into sign, exponent, significand parts.
	const rep_t aRep = toRep(x);
	const rep_t aAbs = aRep & absMask;
	const fixint_t sign = aRep & signBit ? -1 : 1;
	const int exponent = static_cast<int>((aAbs >> significandBits) - exponentBias);
	const rep_t significand = (aAbs & significandMask) | implicitBit;

	// If exponent is negative, the result is zero.
	if (exponent < 0)
		return 0;

	// If the value is too large for the integer type, saturate.
	if ((unsigned int) exponent >= sizeof(fixint_t) * 8)
		return sign == 1 ? fixint_max : fixint_min;

	// If 0 <= exponent < significandBits, right shift to get the result.
	// Otherwise, shift left.
	if (exponent < significandBits)
		return (fixint_t) (sign * (significand >> (significandBits - exponent)));
	else
		return (fixint_t) (sign * ((fixuint_t) significand << (exponent - significandBits)));
}

static inline fixuint_t __fixuint(tf_float x) {
	using namespace flt128;

	// Break x into sign, exponent, significand parts.
	const rep_t aRep = toRep(x);
	const rep_t aAbs = aRep & absMask;
	const int sign = aRep & signBit ? -1 : 1;
	const int exponent = static_cast<int>((aAbs >> significandBits) - exponentBias);
	const rep_t significand = (aAbs & significandMask) | implicitBit;

	// If either the value or the exponent is negative, the result is zero.
	if (sign == -1 || exponent < 0)
		return 0;

	// If the value is too large for the integer type, saturate.
	if ((unsigned int) exponent >= sizeof(fixuint_t) * 8)
		return ~fixuint_t {0};

	// If 0 <= exponent < significandBits, right shift to get the result.
	// Otherwise, shift left.
	if (exponent < significandBits)
		return (fixuint_t) (significand >> (significandBits - exponent));
	else
		return (fixuint_t) significand << (exponent - significandBits);
}

using di_int = int64_t;
using du_int = uint64_t;

extern "C" di_int __fixtfdi(tf_float x) {
	return __fixint(x);
}

extern "C" tf_float __floatditf(di_int x) {
	using namespace flt128;

	const int xWidth = sizeof(x) * 8;

	// Handle zero as a special case to protect clz
	if (x == 0)
		return fromRep(0);

	// All other cases begin by extracting the sign and absolute value of x
	rep_t sign = 0;
	auto xAbs = (du_int) x;
	if (x < 0) {
		sign = signBit;
		xAbs = ~(du_int) x + 1U;
	}

	// Exponent of (fp_t) x is the width of abs(x).
	const int exponent = (xWidth - 1) - __builtin_clzll(xAbs);
	rep_t result;

	// Shift x into the significand field, rounding if it is a right-shift
	const int shift = significandBits - exponent;
	result = (rep_t) xAbs << shift ^ implicitBit;

	// Insert the exponent
	result += (rep_t) (exponent + exponentBias) << significandBits;
	// Insert the sign bit and return
	return fromRep(result | sign);
}

static inline tf_float __addXf3__(tf_float a, tf_float b) {
	using namespace flt128;

	rep_t aRep = toRep(a);
	rep_t bRep = toRep(b);
	const rep_t aAbs = aRep & absMask;
	const rep_t bAbs = bRep & absMask;

	// Detect if a or b is zero, infinity, or NaN.
	if (aAbs - rep_t {1} >= infRep - rep_t {1} ||
		bAbs - rep_t {1} >= infRep - rep_t {1}) {
		// NaN + anything = qNaN
		if (aAbs > infRep)
			return fromRep(toRep(a) | quietBit);
		// anything + NaN = qNaN
		if (bAbs > infRep)
			return fromRep(toRep(b) | quietBit);

		if (aAbs == infRep) {
			// +/-infinity + -/+infinity = qNaN
			if ((toRep(a) ^ toRep(b)) == signBit)
				return fromRep(qnanRep);
				// +/-infinity + anything remaining = +/- infinity
			else
				return a;
		}

		// anything remaining + +/-infinity = +/-infinity
		if (bAbs == infRep)
			return b;

		// zero + anything = anything
		if (!aAbs) {
			// We need to get the sign right for zero + zero.
			if (!bAbs)
				return fromRep(toRep(a) & toRep(b));
			else
				return b;
		}

		// anything + zero = anything
		if (!bAbs)
			return a;
	}

	// Swap a and b if necessary so that a has the larger absolute value.
	if (bAbs > aAbs) {
		const rep_t temp = aRep;
		aRep = bRep;
		bRep = temp;
	}

	// Extract the exponent and significand from the (possibly swapped) a and b.
	int aExponent = static_cast<int>(aRep >> significandBits & maxExponent);
	int bExponent = static_cast<int>(bRep >> significandBits & maxExponent);
	rep_t aSignificand = aRep & significandMask;
	rep_t bSignificand = bRep & significandMask;

	// Normalize any denormals, and adjust the exponent accordingly.
	if (aExponent == 0)
		aExponent = normalize(&aSignificand);
	if (bExponent == 0)
		bExponent = normalize(&bSignificand);

	// The sign of the result is the sign of the larger operand, a.  If they
	// have opposite signs, we are performing a subtraction.  Otherwise, we
	// perform addition.
	const rep_t resultSign = aRep & signBit;
	const bool subtraction = (aRep ^ bRep) & signBit;

	// Shift the significands to give us round, guard and sticky, and set the
	// implicit significand bit.  If we fell through from the denormal path it
	// was already set by normalize( ), but setting it twice won't hurt
	// anything.
	aSignificand = (aSignificand | implicitBit) << 3;
	bSignificand = (bSignificand | implicitBit) << 3;

	// Shift the significand of b by the difference in exponents, with a sticky
	// bottom bit to get rounding correct.
	const auto align = (unsigned int) (aExponent - bExponent);
	if (align) {
		if (align < typeWidth) {
			const bool sticky = (bSignificand << (typeWidth - align)) != 0;
			bSignificand = bSignificand >> align | sticky;
		}
		else {
			bSignificand = 1; // Set the sticky bit.  b is known to be non-zero.
		}
	}
	if (subtraction) {
		aSignificand -= bSignificand;
		// If a == -b, return +zero.
		if (aSignificand == 0)
			return fromRep(0);

		// If partial cancellation occured, we need to left-shift the result
		// and adjust the exponent.
		if (aSignificand < implicitBit << 3) {
			const int shift = rep_clz(aSignificand) - rep_clz(implicitBit << 3);
			aSignificand <<= shift;
			aExponent -= shift;
		}
	}
	else /* addition */ {
		aSignificand += bSignificand;

		// If the addition carried up, we need to right-shift the result and
		// adjust the exponent.
		if (aSignificand & implicitBit << 4) {
			const bool sticky = aSignificand & 1;
			aSignificand = aSignificand >> 1 | sticky;
			aExponent += 1;
		}
	}

	// If we have overflowed the type, return +/- infinity.
	if (aExponent >= maxExponent)
		return fromRep(infRep | resultSign);

	if (aExponent <= 0) {
		// The result is denormal before rounding.  The exponent is zero and we
		// need to shift the significand.
		const int shift = 1 - aExponent;
		const bool sticky = (aSignificand << (typeWidth - shift)) != 0;
		aSignificand = aSignificand >> shift | sticky;
		aExponent = 0;
	}

	// Low three bits are round, guard, and sticky.
	const int roundGuardSticky = static_cast<int>(aSignificand & 0x7);

	// Shift the significand into place, and mask off the implicit bit.
	rep_t result = aSignificand >> 3 & significandMask;

	// Insert the exponent and sign.
	result |= (rep_t) aExponent << significandBits;
	result |= resultSign;

	// Perform the final rounding.  The result may overflow to infinity, but
	// that is the correct result in that case.
	switch (fegetround()) {
		case FE_TONEAREST:
			if (roundGuardSticky > 0x4)
				result++;
			if (roundGuardSticky == 0x4)
				result += result & 1;
			break;
		case FE_DOWNWARD:
			if (resultSign && roundGuardSticky) result++;
			break;
		case FE_UPWARD:
			if (!resultSign && roundGuardSticky) result++;
			break;
		case FE_TOWARDZERO:
			break;
	}
	if (roundGuardSticky)
		feraiseexcept(FE_INEXACT);
	return fromRep(result);
}

extern "C" tf_float __addtf3(tf_float a, tf_float b) {
	return __addXf3__(a, b);
}

extern "C" tf_float __subtf3(tf_float a, tf_float b) {
	using namespace flt128;
	return __addtf3(a, fromRep(toRep(b) ^ signBit));
}

static inline tf_float __mulXf3__(tf_float a, tf_float b) {
	using namespace flt128;

	const unsigned int aExponent = toRep(a) >> significandBits & maxExponent;
	const unsigned int bExponent = toRep(b) >> significandBits & maxExponent;
	const rep_t productSign = (toRep(a) ^ toRep(b)) & signBit;

	rep_t aSignificand = toRep(a) & significandMask;
	rep_t bSignificand = toRep(b) & significandMask;
	int scale = 0;

	// Detect if a or b is zero, denormal, infinity, or NaN.
	if (aExponent - 1U >= maxExponent - 1U ||
		bExponent - 1U >= maxExponent - 1U) {

		const rep_t aAbs = toRep(a) & absMask;
		const rep_t bAbs = toRep(b) & absMask;

		// NaN * anything = qNaN
		if (aAbs > infRep)
			return fromRep(toRep(a) | quietBit);
		// anything * NaN = qNaN
		if (bAbs > infRep)
			return fromRep(toRep(b) | quietBit);

		if (aAbs == infRep) {
			// infinity * non-zero = +/- infinity
			if (bAbs)
				return fromRep(aAbs | productSign);
				// infinity * zero = NaN
			else
				return fromRep(qnanRep);
		}

		if (bAbs == infRep) {
			// non-zero * infinity = +/- infinity
			if (aAbs)
				return fromRep(bAbs | productSign);
				// zero * infinity = NaN
			else
				return fromRep(qnanRep);
		}

		// zero * anything = +/- zero
		if (!aAbs)
			return fromRep(productSign);
		// anything * zero = +/- zero
		if (!bAbs)
			return fromRep(productSign);

		// One or both of a or b is denormal.  The other (if applicable) is a
		// normal number.  Renormalize one or both of a and b, and set scale to
		// include the necessary exponent adjustment.
		if (aAbs < implicitBit)
			scale += normalize(&aSignificand);
		if (bAbs < implicitBit)
			scale += normalize(&bSignificand);
	}

	// Set the implicit significand bit.  If we fell through from the
	// denormal path it was already set by normalize( ), but setting it twice
	// won't hurt anything.
	aSignificand |= implicitBit;
	bSignificand |= implicitBit;

	// Perform a basic multiplication on the significands.  One of them must be
	// shifted beforehand to be aligned with the exponent.
	rep_t productHi, productLo;
	wideMultiply(aSignificand, bSignificand << exponentBits, &productHi,
				 &productLo);

	int productExponent = static_cast<int>(aExponent + bExponent - exponentBias + scale);

	// Normalize the significand and adjust the exponent if needed.
	if (productHi & implicitBit)
		productExponent++;
	else
		wideLeftShift(&productHi, &productLo, 1);

	// If we have overflowed the type, return +/- infinity.
	if (productExponent >= maxExponent)
		return fromRep(infRep | productSign);

	if (productExponent <= 0) {
		// The result is denormal before rounding.
		//
		// If the result is so small that it just underflows to zero, return
		// zero with the appropriate sign.  Mathematically, there is no need to
		// handle this case separately, but we make it a special case to
		// simplify the shift logic.
		const unsigned int shift = rep_t {1} - (unsigned int)productExponent;
		if (shift >= typeWidth)
			return fromRep(productSign);

		// Otherwise, shift the significand of the result so that the round
		// bit is the high bit of productLo.
		wideRightShiftWithSticky(&productHi, &productLo, shift);
	}
	else {
		// The result is normal before rounding.  Insert the exponent.
		productHi &= significandMask;
		productHi |= (rep_t)productExponent << significandBits;
	}

	// Insert the sign of the result.
	productHi |= productSign;

	// Perform the final rounding.  The final result may overflow to infinity,
	// or underflow to zero, but those are the correct results in those cases.
	// We use the default IEEE-754 round-to-nearest, ties-to-even rounding mode.
	if (productLo > signBit)
		productHi++;
	if (productLo == signBit)
		productHi += productHi & 1;
	return fromRep(productHi);
}

extern "C" tf_float __multf3(tf_float a, tf_float b) {
	return __mulXf3__(a, b);
}

using si_int = int32_t;
using su_int = uint32_t;

extern "C" tf_float __floatsitf(si_int a) {
	using namespace flt128;

	const int aWidth = sizeof(a) * 8;

	// Handle zero as a special case to protect clz
	if (a == 0)
		return fromRep(0);

	// All other cases begin by extracting the sign and absolute value of a
	rep_t sign = 0;
	auto aAbs = (su_int) a;
	if (a < 0) {
		sign = signBit;
		aAbs = -aAbs;
	}

	// Exponent of (fp_t) a is the width of abs(a).
	const int exponent = (aWidth - 1) - clzsi(aAbs);
	rep_t result;

	// Shift a into the significand field and clear the implicit bit.
	const int shift = significandBits - exponent;
	result = (rep_t) aAbs << shift ^ implicitBit;

	// Insert the exponent
	result += (rep_t) (exponent + exponentBias) << significandBits;
	// Insert the sign bit and return
	return fromRep(result | sign);
}

// Force unrolling the code specified to be repeated N times.
#define REPEAT_0_TIMES(code_to_repeat) /* do nothing */
#define REPEAT_1_TIMES(code_to_repeat) code_to_repeat
#define REPEAT_2_TIMES(code_to_repeat)                                         \
  REPEAT_1_TIMES(code_to_repeat)                                               \
  code_to_repeat
#define REPEAT_3_TIMES(code_to_repeat)                                         \
  REPEAT_2_TIMES(code_to_repeat)                                               \
  code_to_repeat
#define REPEAT_4_TIMES(code_to_repeat)                                         \
  REPEAT_3_TIMES(code_to_repeat)                                               \
  code_to_repeat

#define REPEAT_N_TIMES_(N, code_to_repeat) REPEAT_##N##_TIMES(code_to_repeat)
#define REPEAT_N_TIMES(N, code_to_repeat) REPEAT_N_TIMES_(N, code_to_repeat)

#define NUMBER_OF_HALF_ITERATIONS 4
#define NUMBER_OF_FULL_ITERATIONS 1

#define HW (typeWidth / 2)
// rep_t-sized bitmask with lower half of bits set to ones
#define loMask (rep_t(-1) >> HW)

#if NUMBER_OF_FULL_ITERATIONS < 1
#error At least one full iteration is required
#endif

static inline tf_float __divXf3__(tf_float a, tf_float b) {
	using namespace flt128;

	const unsigned int aExponent = toRep(a) >> significandBits & maxExponent;
	const unsigned int bExponent = toRep(b) >> significandBits & maxExponent;
	const rep_t quotientSign = (toRep(a) ^ toRep(b)) & signBit;

	rep_t aSignificand = toRep(a) & significandMask;
	rep_t bSignificand = toRep(b) & significandMask;
	int scale = 0;

	// Detect if a or b is zero, denormal, infinity, or NaN.
	if (aExponent - 1U >= maxExponent - 1U ||
	    bExponent - 1U >= maxExponent - 1U) {

		const rep_t aAbs = toRep(a) & absMask;
		const rep_t bAbs = toRep(b) & absMask;

		// NaN / anything = qNaN
		if (aAbs > infRep)
			return fromRep(toRep(a) | quietBit);
		// anything / NaN = qNaN
		if (bAbs > infRep)
			return fromRep(toRep(b) | quietBit);

		if (aAbs == infRep) {
			// infinity / infinity = NaN
			if (bAbs == infRep)
				return fromRep(qnanRep);
				// infinity / anything else = +/- infinity
			else
				return fromRep(aAbs | quotientSign);
		}

		// anything else / infinity = +/- 0
		if (bAbs == infRep)
			return fromRep(quotientSign);

		if (!aAbs) {
			// zero / zero = NaN
			if (!bAbs)
				return fromRep(qnanRep);
				// zero / anything else = +/- zero
			else
				return fromRep(quotientSign);
		}
		// anything else / zero = +/- infinity
		if (!bAbs)
			return fromRep(infRep | quotientSign);

		// One or both of a or b is denormal.  The other (if applicable) is a
		// normal number.  Renormalize one or both of a and b, and set scale to
		// include the necessary exponent adjustment.
		if (aAbs < implicitBit)
			scale += normalize(&aSignificand);
		if (bAbs < implicitBit)
			scale -= normalize(&bSignificand);
	}

	// Set the implicit significand bit.  If we fell through from the
	// denormal path it was already set by normalize( ), but setting it twice
	// won't hurt anything.
	aSignificand |= implicitBit;
	bSignificand |= implicitBit;

	int writtenExponent = static_cast<int>((aExponent - bExponent + scale) + exponentBias);

	const rep_t b_UQ1 = bSignificand << (typeWidth - significandBits - 1);

	// Align the significand of b as a UQ1.(n-1) fixed-point number in the range
	// [1.0, 2.0) and get a UQ0.n approximate reciprocal using a small minimax
	// polynomial approximation: x0 = 3/4 + 1/sqrt(2) - b/2.
	// The max error for this approximation is achieved at endpoints, so
	//   abs(x0(b) - 1/b) <= abs(x0(1) - 1/1) = 3/4 - 1/sqrt(2) = 0.04289...,
	// which is about 4.5 bits.
	// The initial approximation is between x0(1.0) = 0.9571... and x0(2.0) = 0.4571...

	// Then, refine the reciprocal estimate using a quadratically converging
	// Newton-Raphson iteration:
	//     x_{n+1} = x_n * (2 - x_n * b)
	//
	// Let b be the original divisor considered "in infinite precision" and
	// obtained from IEEE754 representation of function argument (with the
	// implicit bit set). Corresponds to rep_t-sized b_UQ1 represented in
	// UQ1.(W-1).
	//
	// Let b_hw be an infinitely precise number obtained from the highest (HW-1)
	// bits of divisor significand (with the implicit bit set). Corresponds to
	// half_rep_t-sized b_UQ1_hw represented in UQ1.(HW-1) that is a **truncated**
	// version of b_UQ1.
	//
	// Let e_n := x_n - 1/b_hw
	//     E_n := x_n - 1/b
	// abs(E_n) <= abs(e_n) + (1/b_hw - 1/b)
	//           = abs(e_n) + (b - b_hw) / (b*b_hw)
	//          <= abs(e_n) + 2 * 2^-HW

	// rep_t-sized iterations may be slower than the corresponding half-width
	// variant depending on the handware and whether single/double/quad precision
	// is selected.
	// NB: Using half-width iterations increases computation errors due to
	// rounding, so error estimations have to be computed taking the selected
	// mode into account!
#if NUMBER_OF_HALF_ITERATIONS > 0
	// Starting with (n-1) half-width iterations
  const half_rep_t b_UQ1_hw = bSignificand >> (significandBits + 1 - HW);

  // C is (3/4 + 1/sqrt(2)) - 1 truncated to W0 fractional bits as UQ0.HW
  // with W0 being either 16 or 32 and W0 <= HW.
  // That is, C is the aforementioned 3/4 + 1/sqrt(2) constant (from which
  // b/2 is subtracted to obtain x0) wrapped to [0, 1) range.
#if defined(SINGLE_PRECISION)
  // Use 16-bit initial estimation in case we are using half-width iterations
  // for float32 division. This is expected to be useful for some 16-bit
  // targets. Not used by default as it requires performing more work during
  // rounding and would hardly help on regular 32- or 64-bit targets.
  const half_rep_t C_hw = HALF_REP_C(0x7504);
#else
  // HW is at least 32. Shifting into the highest bits if needed.
  const half_rep_t C_hw = half_rep_t {0x7504F333} << (HW - 32);
#endif

  // b >= 1, thus an upper bound for 3/4 + 1/sqrt(2) - b/2 is about 0.9572,
  // so x0 fits to UQ0.HW without wrapping.
  half_rep_t x_UQ0_hw = C_hw - (b_UQ1_hw /* exact b_hw/2 as UQ0.HW */);
  // An e_0 error is comprised of errors due to
  // * x0 being an inherently imprecise first approximation of 1/b_hw
  // * C_hw being some (irrational) number **truncated** to W0 bits
  // Please note that e_0 is calculated against the infinitely precise
  // reciprocal of b_hw (that is, **truncated** version of b).
  //
  // e_0 <= 3/4 - 1/sqrt(2) + 2^-W0

  // By construction, 1 <= b < 2
  // f(x)  = x * (2 - b*x) = 2*x - b*x^2
  // f'(x) = 2 * (1 - b*x)
  //
  // On the [0, 1] interval, f(0)   = 0,
  // then it increses until  f(1/b) = 1 / b, maximum on (0, 1),
  // then it decreses to     f(1)   = 2 - b
  //
  // Let g(x) = x - f(x) = b*x^2 - x.
  // On (0, 1/b), g(x) < 0 <=> f(x) > x
  // On (1/b, 1], g(x) > 0 <=> f(x) < x
  //
  // For half-width iterations, b_hw is used instead of b.
  REPEAT_N_TIMES(NUMBER_OF_HALF_ITERATIONS, {
    // corr_UQ1_hw can be **larger** than 2 - b_hw*x by at most 1*Ulp
    // of corr_UQ1_hw.
    // "0.0 - (...)" is equivalent to "2.0 - (...)" in UQ1.(HW-1).
    // On the other hand, corr_UQ1_hw should not overflow from 2.0 to 0.0 provided
    // no overflow occurred earlier: ((rep_t)x_UQ0_hw * b_UQ1_hw >> HW) is
    // expected to be strictly positive because b_UQ1_hw has its highest bit set
    // and x_UQ0_hw should be rather large (it converges to 1/2 < 1/b_hw <= 1).
    half_rep_t corr_UQ1_hw = 0 - ((rep_t)x_UQ0_hw * b_UQ1_hw >> HW);

    // Now, we should multiply UQ0.HW and UQ1.(HW-1) numbers, naturally
    // obtaining an UQ1.(HW-1) number and proving its highest bit could be
    // considered to be 0 to be able to represent it in UQ0.HW.
    // From the above analysis of f(x), if corr_UQ1_hw would be represented
    // without any intermediate loss of precision (that is, in twice_rep_t)
    // x_UQ0_hw could be at most [1.]000... if b_hw is exactly 1.0 and strictly
    // less otherwise. On the other hand, to obtain [1.]000..., one have to pass
    // 1/b_hw == 1.0 to f(x), so this cannot occur at all without overflow (due
    // to 1.0 being not representable as UQ0.HW).
    // The fact corr_UQ1_hw was virtually round up (due to result of
    // multiplication being **first** truncated, then negated - to improve
    // error estimations) can increase x_UQ0_hw by up to 2*Ulp of x_UQ0_hw.
    x_UQ0_hw = (rep_t)x_UQ0_hw * corr_UQ1_hw >> (HW - 1);
    // Now, either no overflow occurred or x_UQ0_hw is 0 or 1 in its half_rep_t
    // representation. In the latter case, x_UQ0_hw will be either 0 or 1 after
    // any number of iterations, so just subtract 2 from the reciprocal
    // approximation after last iteration.

    // In infinite precision, with 0 <= eps1, eps2 <= U = 2^-HW:
    // corr_UQ1_hw = 2 - (1/b_hw + e_n) * b_hw + 2*eps1
    //             = 1 - e_n * b_hw + 2*eps1
    // x_UQ0_hw = (1/b_hw + e_n) * (1 - e_n*b_hw + 2*eps1) - eps2
    //          = 1/b_hw - e_n + 2*eps1/b_hw + e_n - e_n^2*b_hw + 2*e_n*eps1 - eps2
    //          = 1/b_hw + 2*eps1/b_hw - e_n^2*b_hw + 2*e_n*eps1 - eps2
    // e_{n+1} = -e_n^2*b_hw + 2*eps1/b_hw + 2*e_n*eps1 - eps2
    //         = 2*e_n*eps1 - (e_n^2*b_hw + eps2) + 2*eps1/b_hw
    //                        \------ >0 -------/   \-- >0 ---/
    // abs(e_{n+1}) <= 2*abs(e_n)*U + max(2*e_n^2 + U, 2 * U)
  })
  // For initial half-width iterations, U = 2^-HW
  // Let  abs(e_n)     <= u_n * U,
  // then abs(e_{n+1}) <= 2 * u_n * U^2 + max(2 * u_n^2 * U^2 + U, 2 * U)
  // u_{n+1} <= 2 * u_n * U + max(2 * u_n^2 * U + 1, 2)

  // Account for possible overflow (see above). For an overflow to occur for the
  // first time, for "ideal" corr_UQ1_hw (that is, without intermediate
  // truncation), the result of x_UQ0_hw * corr_UQ1_hw should be either maximum
  // value representable in UQ0.HW or less by 1. This means that 1/b_hw have to
  // be not below that value (see g(x) above), so it is safe to decrement just
  // once after the final iteration. On the other hand, an effective value of
  // divisor changes after this point (from b_hw to b), so adjust here.
  x_UQ0_hw -= 1U;
  rep_t x_UQ0 = (rep_t)x_UQ0_hw << HW;
  x_UQ0 -= 1U;

#else
	// C is (3/4 + 1/sqrt(2)) - 1 truncated to 32 fractional bits as UQ0.n
	const rep_t C = REP_C(0x7504F333) << (typeWidth - 32);
	rep_t x_UQ0 = C - b_UQ1;
	// E_0 <= 3/4 - 1/sqrt(2) + 2 * 2^-32
#endif

	// Error estimations for full-precision iterations are calculated just
	// as above, but with U := 2^-W and taking extra decrementing into account.
	// We need at least one such iteration.

#ifdef USE_NATIVE_FULL_ITERATIONS
	REPEAT_N_TIMES(NUMBER_OF_FULL_ITERATIONS, {
    rep_t corr_UQ1 = 0 - ((twice_rep_t)x_UQ0 * b_UQ1 >> typeWidth);
    x_UQ0 = (twice_rep_t)x_UQ0 * corr_UQ1 >> (typeWidth - 1);
  })
#else
#if NUMBER_OF_FULL_ITERATIONS != 1
#error Only a single emulated full iteration is supported
#endif
#if !(NUMBER_OF_HALF_ITERATIONS > 0)
	// Cannot normally reach here: only one full-width iteration is requested and
	// the total number of iterations should be at least 3 even for float32.
#error Check NUMBER_OF_HALF_ITERATIONS, NUMBER_OF_FULL_ITERATIONS and USE_NATIVE_FULL_ITERATIONS.
#endif
	// Simulating operations on a twice_rep_t to perform a single final full-width
	// iteration. Using ad-hoc multiplication implementations to take advantage
	// of particular structure of operands.
	rep_t blo = b_UQ1 & loMask;
	// x_UQ0 = x_UQ0_hw * 2^HW - 1
	// x_UQ0 * b_UQ1 = (x_UQ0_hw * 2^HW) * (b_UQ1_hw * 2^HW + blo) - b_UQ1
	//
	//   <--- higher half ---><--- lower half --->
	//   [x_UQ0_hw * b_UQ1_hw]
	// +            [  x_UQ0_hw *  blo  ]
	// -                      [      b_UQ1       ]
	// = [      result       ][.... discarded ...]
	rep_t corr_UQ1 = 0U - (   (rep_t)x_UQ0_hw * b_UQ1_hw
	                          + ((rep_t)x_UQ0_hw * blo >> HW)
	                          - rep_t {1}); // account for *possible* carry
	rep_t lo_corr = corr_UQ1 & loMask;
	rep_t hi_corr = corr_UQ1 >> HW;
	// x_UQ0 * corr_UQ1 = (x_UQ0_hw * 2^HW) * (hi_corr * 2^HW + lo_corr) - corr_UQ1
	x_UQ0 =   ((rep_t)x_UQ0_hw * hi_corr << 1)
	          + ((rep_t)x_UQ0_hw * lo_corr >> (HW - 1))
	          - rep_t {2}; // 1 to account for the highest bit of corr_UQ1 can be 1
	// 1 to account for possible carry
	// Just like the case of half-width iterations but with possibility
	// of overflowing by one extra Ulp of x_UQ0.
	x_UQ0 -= 1U;
	// ... and then traditional fixup by 2 should work

	// On error estimation:
	// abs(E_{N-1}) <=   (u_{N-1} + 2 /* due to conversion e_n -> E_n */) * 2^-HW
	//                 + (2^-HW + 2^-W))
	// abs(E_{N-1}) <= (u_{N-1} + 3.01) * 2^-HW

	// Then like for the half-width iterations:
	// With 0 <= eps1, eps2 < 2^-W
	// E_N  = 4 * E_{N-1} * eps1 - (E_{N-1}^2 * b + 4 * eps2) + 4 * eps1 / b
	// abs(E_N) <= 2^-W * [ 4 * abs(E_{N-1}) + max(2 * abs(E_{N-1})^2 * 2^W + 4, 8)) ]
	// abs(E_N) <= 2^-W * [ 4 * (u_{N-1} + 3.01) * 2^-HW + max(4 + 2 * (u_{N-1} + 3.01)^2, 8) ]
#endif

	// Finally, account for possible overflow, as explained above.
	x_UQ0 -= 2U;

	// u_n for different precisions (with N-1 half-width iterations):
	// W0 is the precision of C
	//   u_0 = (3/4 - 1/sqrt(2) + 2^-W0) * 2^HW

	// Estimated with bc:
	//   define half1(un) { return 2.0 * (un + un^2) / 2.0^hw + 1.0; }
	//   define half2(un) { return 2.0 * un / 2.0^hw + 2.0; }
	//   define full1(un) { return 4.0 * (un + 3.01) / 2.0^hw + 2.0 * (un + 3.01)^2 + 4.0; }
	//   define full2(un) { return 4.0 * (un + 3.01) / 2.0^hw + 8.0; }

	//             | f32 (0 + 3) | f32 (2 + 1)  | f64 (3 + 1)  | f128 (4 + 1)
	// u_0         | < 184224974 | < 2812.1     | < 184224974  | < 791240234244348797
	// u_1         | < 15804007  | < 242.7      | < 15804007   | < 67877681371350440
	// u_2         | < 116308    | < 2.81       | < 116308     | < 499533100252317
	// u_3         | < 7.31      |              | < 7.31       | < 27054456580
	// u_4         |             |              |              | < 80.4
	// Final (U_N) | same as u_3 | < 72         | < 218        | < 13920

	// Add 2 to U_N due to final decrement.

#if NUMBER_OF_HALF_ITERATIONS == 4 && NUMBER_OF_FULL_ITERATIONS == 1
#define RECIPROCAL_PRECISION rep_t {13922}
#else
#error Invalid number of iterations
#endif

	// Suppose 1/b - P * 2^-W < x < 1/b + P * 2^-W
	x_UQ0 -= RECIPROCAL_PRECISION;
	// Now 1/b - (2*P) * 2^-W < x < 1/b
	// FIXME Is x_UQ0 still >= 0.5?

	rep_t quotient_UQ1, dummy;
	wideMultiply(x_UQ0, aSignificand << 1, &quotient_UQ1, &dummy);
	// Now, a/b - 4*P * 2^-W < q < a/b for q=<quotient_UQ1:dummy> in UQ1.(SB+1+W).

	// quotient_UQ1 is in [0.5, 2.0) as UQ1.(SB+1),
	// adjust it to be in [1.0, 2.0) as UQ1.SB.
	rep_t residualLo;
	if (quotient_UQ1 < (implicitBit << 1)) {
		// Highest bit is 0, so just reinterpret quotient_UQ1 as UQ1.SB,
		// effectively doubling its value as well as its error estimation.
		residualLo = (aSignificand << (significandBits + 1)) - quotient_UQ1 * bSignificand;
		writtenExponent -= 1;
		aSignificand <<= 1;
	} else {
		// Highest bit is 1 (the UQ1.(SB+1) value is in [1, 2)), convert it
		// to UQ1.SB by right shifting by 1. Least significant bit is omitted.
		quotient_UQ1 >>= 1;
		residualLo = (aSignificand << significandBits) - quotient_UQ1 * bSignificand;
	}
	// NB: residualLo is calculated above for the normal result case.
	//     It is re-computed on denormal path that is expected to be not so
	//     performance-sensitive.

	// Now, q cannot be greater than a/b and can differ by at most 8*P * 2^-W + 2^-SB
	// Each NextAfter() increments the floating point value by at least 2^-SB
	// (more, if exponent was incremented).
	// Different cases (<---> is of 2^-SB length, * = a/b that is shown as a midpoint):
	//   q
	//   |   | * |   |   |       |       |
	//       <--->      2^t
	//   |   |   |   |   |   *   |       |
	//               q
	// To require at most one NextAfter(), an error should be less than 1.5 * 2^-SB.
	//   (8*P) * 2^-W + 2^-SB < 1.5 * 2^-SB
	//   (8*P) * 2^-W         < 0.5 * 2^-SB
	//   P < 2^(W-4-SB)
	// Generally, for at most R NextAfter() to be enough,
	//   P < (2*R - 1) * 2^(W-4-SB)
	// For f32 (0+3): 10 < 32 (OK)
	// For f32 (2+1): 32 < 74 < 32 * 3, so two NextAfter() are required
	// For f64: 220 < 256 (OK)
	// For f128: 4096 * 3 < 13922 < 4096 * 5 (three NextAfter() are required)

	// If we have overflowed the exponent, return infinity
	if (writtenExponent >= maxExponent)
		return fromRep(infRep | quotientSign);

	// Now, quotient_UQ1_SB <= the correctly-rounded result
	// and may need taking NextAfter() up to 3 times (see error estimates above)
	// r = a - b * q
	rep_t absResult;
	if (writtenExponent > 0) {
		// Clear the implicit bit
		absResult = quotient_UQ1 & significandMask;
		// Insert the exponent
		absResult |= (rep_t)writtenExponent << significandBits;
		residualLo <<= 1;
	} else {
		// Prevent shift amount from being negative
		if (significandBits + writtenExponent < 0)
			return fromRep(quotientSign);

		absResult = quotient_UQ1 >> (-writtenExponent + 1);

		// multiplied by two to prevent shift amount to be negative
		residualLo = (aSignificand << (significandBits + writtenExponent)) - (absResult * bSignificand << 1);
	}

	// Round
	residualLo += absResult & 1; // tie to even
	// The above line conditionally turns the below LT comparison into LTE
	absResult += residualLo > bSignificand;
	// Do not round Infinity to NaN
	absResult += absResult < infRep && residualLo > (2 + 1) * bSignificand;
	absResult += absResult < infRep && residualLo > (4 + 1) * bSignificand;
	return fromRep(absResult | quotientSign);
}

extern "C" tf_float __divtf3(tf_float a, tf_float b) {
	return __divXf3__(a, b);
}

template<typename Dst>
static inline Dst __truncXfYf2__(tf_float a) {
	using namespace flt128;

	constexpr int dstExpBits = FltStats<Dst>::ExpBits;
	constexpr int dstSigFracBits = FltStats<Dst>::SigFracBits;

	constexpr int srcExpBits = FltStats<tf_float>::ExpBits;
	constexpr int srcBits = sizeof(tf_float) * 8;
	constexpr int dstBits = sizeof(Dst) * 8;
	constexpr int srcSigFracBits = FltStats<tf_float>::SigFracBits;
	using src_rep_t = FltStats<tf_float>::rep_t;
	using dst_rep_t = FltStats<Dst>::rep_t;

	auto srcToRep = [](tf_float x) {
		return hz::bit_cast<src_rep_t>(x);
	};

	auto dstFromRep = [](dst_rep_t x) {
		return hz::bit_cast<Dst>(x);
	};

	auto extract_sign_from_src = [](src_rep_t x) {
		const src_rep_t srcSignMask = src_rep_t {1} << (srcBits - 1);
		return (x & srcSignMask) >> (srcBits - 1);
	};

	auto extract_exp_from_src = [](src_rep_t x) {
		const int srcSigBits = srcBits - 1 - srcExpBits;
		const src_rep_t srcExpMask = ((src_rep_t {1} << srcExpBits) - 1) << srcSigBits;
		return (x & srcExpMask) >> srcSigBits;
	};

	auto extract_sig_frac_from_src = [](src_rep_t x) {
		const src_rep_t srcSigFracMask = (src_rep_t {1} << srcSigFracBits) - 1;
		return x & srcSigFracMask;
	};

	auto construct_dst_rep = [](dst_rep_t sign, dst_rep_t exp, dst_rep_t sigFrac) {
		return (sign << (dstBits - 1)) | (exp << (dstBits - 1 - dstExpBits)) | sigFrac;
	};

	// Various constants whose values follow from the type parameters.
	// Any reasonable optimizer will fold and propagate all of these.
	const int srcInfExp = (1 << srcExpBits) - 1;
	const int srcExpBias = srcInfExp >> 1;

	const src_rep_t srcMinNormal = src_rep_t {1} << srcSigFracBits;
	const src_rep_t roundMask =
		(src_rep_t {1} << (srcSigFracBits - dstSigFracBits)) - 1;
	const src_rep_t halfway = src_rep_t {1}
		<< (srcSigFracBits - dstSigFracBits - 1);
	const src_rep_t srcQNaN = src_rep_t {1} << (srcSigFracBits - 1);
	const src_rep_t srcNaNCode = srcQNaN - 1;

	const int dstInfExp = (1 << dstExpBits) - 1;
	const int dstExpBias = dstInfExp >> 1;
	const int overflowExponent = srcExpBias + dstInfExp - dstExpBias;

	const dst_rep_t dstQNaN = dst_rep_t {1} << (dstSigFracBits - 1);
	const dst_rep_t dstNaNCode = dstQNaN - 1;

	const src_rep_t aRep = srcToRep(a);
	const src_rep_t srcSign = extract_sign_from_src(aRep);
	const src_rep_t srcExp = extract_exp_from_src(aRep);
	const src_rep_t srcSigFrac = extract_sig_frac_from_src(aRep);

	dst_rep_t dstSign = srcSign;
	dst_rep_t dstExp;
	dst_rep_t dstSigFrac;

	// Same size exponents and a's significand tail is 0.
	// The significand can be truncated and the exponent can be copied over.
	const int sigFracTailBits = srcSigFracBits - dstSigFracBits;
	if (srcExpBits == dstExpBits &&
	    ((aRep >> sigFracTailBits) << sigFracTailBits) == aRep) {
		dstExp = srcExp;
		dstSigFrac = (dst_rep_t)(srcSigFrac >> sigFracTailBits);
		return dstFromRep(construct_dst_rep(dstSign, dstExp, dstSigFrac));
	}

	const int dstExpCandidate = ((int) srcExp - srcExpBias) + dstExpBias;
	if (dstExpCandidate >= 1 && dstExpCandidate < dstInfExp) {
		// The exponent of a is within the range of normal numbers in the
		// destination format. We can convert by simply right-shifting with
		// rounding and adjusting the exponent.
		dstExp = dstExpCandidate;
		dstSigFrac = (dst_rep_t)(srcSigFrac >> sigFracTailBits);

		const src_rep_t roundBits = srcSigFrac & roundMask;
		// Round to nearest.
		if (roundBits > halfway)
			dstSigFrac++;
			// Tie to even.
		else if (roundBits == halfway)
			dstSigFrac += dstSigFrac & 1;

		// Rounding has changed the exponent.
		if (dstSigFrac >= (dst_rep_t {1} << dstSigFracBits)) {
			dstExp += 1;
			dstSigFrac ^= (dst_rep_t {1} << dstSigFracBits);
		}
	}
	else if (srcExp == srcInfExp && srcSigFrac) {
		// a is NaN.
		// Conjure the result by beginning with infinity, setting the qNaN
		// bit and inserting the (truncated) trailing NaN field.
		dstExp = dstInfExp;
		dstSigFrac = dstQNaN;
		dstSigFrac |= ((srcSigFrac & srcNaNCode) >> sigFracTailBits) & dstNaNCode;
	}
	else if ((int) srcExp >= overflowExponent) {
		dstExp = dstInfExp;
		dstSigFrac = 0;
	}
	else {
		// a underflows on conversion to the destination type or is an exact
		// zero.  The result may be a denormal or zero.  Extract the exponent
		// to get the shift amount for the denormalization.
		src_rep_t significand = srcSigFrac;
		int shift = static_cast<int>(srcExpBias - dstExpBias - srcExp);

		if (srcExp) {
			// Set the implicit integer bit if the source is a normal number.
			significand |= srcMinNormal;
			shift += 1;
		}

		// Right shift by the denormalization amount with sticky.
		if (shift > srcSigFracBits) {
			dstExp = 0;
			dstSigFrac = 0;
		}
		else {
			dstExp = 0;
			const bool sticky = shift && ((significand << (srcBits - shift)) != 0);
			src_rep_t denormalizedSignificand = significand >> shift | sticky;
			dstSigFrac = denormalizedSignificand >> sigFracTailBits;
			const src_rep_t roundBits = denormalizedSignificand & roundMask;
			// Round to nearest
			if (roundBits > halfway)
				dstSigFrac++;
				// Ties to even
			else if (roundBits == halfway)
				dstSigFrac += dstSigFrac & 1;

			// Rounding has changed the exponent.
			if (dstSigFrac >= (dst_rep_t {1} << dstSigFracBits)) {
				dstExp += 1;
				dstSigFrac ^= (dst_rep_t {1} << dstSigFracBits);
			}
		}
	}

	return dstFromRep(construct_dst_rep(dstSign, dstExp, dstSigFrac));
}

extern "C" double __trunctfdf2(tf_float a) {
	return __truncXfYf2__<double>(a);
}

extern "C" si_int __fixtfsi(tf_float a) {
	return static_cast<si_int>(__fixint(a));
}

extern "C" float __trunctfsf2(tf_float a) {
	return __truncXfYf2__<float>(a);
}

extern "C" tf_float __extendsftf2(float a) {
	return __extendXfYf2__<float>(a);
}

enum {
	GE_LESS = -1,
	GE_EQUAL = 0,
	GE_GREATER = 1,
	GE_UNORDERED = -1 // Note: different from LE_UNORDERED
};

static inline CMP_RESULT __geXf2__(tf_float a, tf_float b) {
	using namespace flt128;

	const auto aInt = static_cast<srep_t>(toRep(a));
	const auto bInt = static_cast<srep_t>(toRep(b));
	const rep_t aAbs = aInt & absMask;
	const rep_t bAbs = bInt & absMask;

	if (aAbs > infRep || bAbs > infRep)
		return GE_UNORDERED;
	if ((aAbs | bAbs) == 0)
		return GE_EQUAL;
	if ((aInt & bInt) >= 0) {
		if (aInt < bInt)
			return GE_LESS;
		else if (aInt == bInt)
			return GE_EQUAL;
		else
			return GE_GREATER;
	}
	else {
		if (aInt > bInt)
			return GE_LESS;
		else if (aInt == bInt)
			return GE_EQUAL;
		else
			return GE_GREATER;
	}
}

extern "C" CMP_RESULT __getf2(tf_float a, tf_float b) {
	return __geXf2__(a, b);
}

extern "C" [[gnu::alias("__getf2")]] typeof(__getf2) __gttf2;

extern "C" tf_float __floatunsitf(su_int x) {
	using namespace flt128;

	constexpr int x_width = sizeof(x) * 8;
	if (x == 0) {
		return fromRep(0);
	}

	const int exponent = (x_width - 1) - clzsi(x);

	const int shift = significandBits - exponent;
	rep_t result = (static_cast<rep_t>(x) << shift) ^ implicitBit;

	result += static_cast<rep_t>(exponent + exponentBias) << significandBits;
	return fromRep(result);
}

extern "C" su_int __fixunstfsi(tf_float x) {
	return __fixuint(x);
}
