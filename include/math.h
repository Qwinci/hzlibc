#ifndef _MATH_H
#define _MATH_H

#include <bits/utils.h>

__begin_decls

#if defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ == 2
typedef long double float_t;
typedef long double double_t;
#else
typedef float float_t;
typedef double double_t;
#endif

#define NAN __builtin_nanf("")
#define INFINITY __builtin_inff()

#define HUGE_VALF __builtin_hugevalf()
#define HUGE_VAL __builtin_hugeval()
#define HUGE_VALL __builtin_hugevall()

#define FP_NAN 0
#define FP_INFINITE 1
#define FP_ZERO 2
#define FP_SUBNORMAL 3
#define FP_NORMAL 4

#define FP_ILOGBNAN (-1 - 0x7FFFFFFF)
#define FP_ILOGB0 FP_ILOGBNAN

#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2

#define __MATH_TG(fn, x) ( \
	sizeof(x) == sizeof(float) ? fn ## f(x) : \
	sizeof(x) == sizeof(double) ? fn(x) : fn ## l(x) \
	)

int __fpclassifyf(float __x);
int __fpclassify(double __x);
int __fpclassifyl(long double __x);

#define signbit(x) __builtin_signbit(x)

#define fpclassify(x) __MATH_TG(__fpclassify, (x))

#define isinf(x) (fpclassify(x) == FP_INFINITE)
#define isnan(x) (fpclassify(x) == FP_NAN)
#define isnormal(x) (fpclassify(x) == FP_NORMAL)
#define isfinite(x) (fpclassify(x) > FP_INFINITE)

int __fpclassify(double);
int __fpclassifyf(float);
int __fpclassifyl(long double);

#define isunordered(x,y) (isnan((x)) ? ((void) (y), 1) : isnan((y)))

#define __ISREL_DEF(rel, op, type) \
static __inline int __is##rel(type __x, type __y) \
{ \
	return !isunordered(__x,__y) && __x op __y; \
}

__ISREL_DEF(lessf, <, float_t)
__ISREL_DEF(less, <, double_t)
__ISREL_DEF(lessl, <, long double)
__ISREL_DEF(lessequalf, <=, float_t)
__ISREL_DEF(lessequal, <=, double_t)
__ISREL_DEF(lessequall, <=, long double)
__ISREL_DEF(lessgreaterf, !=, float_t)
__ISREL_DEF(lessgreater, !=, double_t)
__ISREL_DEF(lessgreaterl, !=, long double)
__ISREL_DEF(greaterf, >, float_t)
__ISREL_DEF(greater, >, double_t)
__ISREL_DEF(greaterl, >, long double)
__ISREL_DEF(greaterequalf, >=, float_t)
__ISREL_DEF(greaterequal, >=, double_t)
__ISREL_DEF(greaterequall, >=, long double)

#define __tg_pred_2(x, y, p) ( \
	sizeof((x) + (y)) == sizeof(float) ? p ## f(x, y) : \
	sizeof((x) + (y)) == sizeof(double) ? p(x, y) : \
	p##l(x, y) )

#define isless(x, y) __tg_pred_2(x, y, __isless)
#define islessequal(x, y) __tg_pred_2(x, y, __islessequal)
#define islessgreater(x, y) __tg_pred_2(x, y, __islessgreater)
#define isgreater(x, y) __tg_pred_2(x, y, __isgreater)
#define isgreaterequal(x, y) __tg_pred_2(x, y, __isgreaterequal)

#define MAXFLOAT 3.40282346638528859812e+38F

#define M_E 2.7182818284590452354           /* e */
#define M_LOG2E 1.4426950408889634074       /* log_2 e */
#define M_LOG10E 0.43429448190325182765     /* log_10 e */
#define M_LN2 0.69314718055994530942        /* log_e 2 */
#define M_LN10 2.30258509299404568402       /* log_e 10 */
#define M_PI 3.14159265358979323846         /* pi */
#define M_PI_2 1.57079632679489661923       /* pi/2 */
#define M_PI_4 0.78539816339744830962       /* pi/4 */
#define M_1_PI 0.31830988618379067154       /* 1/pi */
#define M_2_PI 0.63661977236758134308       /* 2/pi */
#define M_2_SQRTPI 1.12837916709551257390   /* 2/sqrt(pi) */
#define M_SQRT2 1.41421356237309504880      /* sqrt(2) */
#define M_SQRT1_2 0.70710678118654752440    /* 1/sqrt(2) */

#define HUGE 3.40282346638528859812e+38F

float powf(float __x, float __y);
double pow(double __x, double __y);
long double powl(long double __x, long double __y);

float pow10f(float __x);
double pow10(double __x);
long double pow10l(long double __x);

float sqrtf(float __x);
double sqrt(double __x);
long double sqrtl(long double __x);

float logf(float __x);
double log(double __x);
long double logl(long double __x);

float log2f(float __x);
double log2(double __x);
long double log2l(long double __x);

float log1pf(float __x);
double log1p(double __x);
long double log1pl(long double __x);

float log10f(float __x);
double log10(double __x);
long double log10l(long double __x);

float logbf(float __x);
double logb(double __x);
long double logbl(long double __x);

float cbrtf(float __x);
double cbrt(double __x);
long double cbrtl(long double __x);

float expf(float __x);
double exp(double __x);
long double expl(long double __x);

float exp2f(float __x);
double exp2(double __x);
long double exp2l(long double __x);

float exp10f(float __x);
double exp10(double __x);
long double exp10l(long double __x);

float expm1f(float __x);
double expm1(double __x);
long double expm1l(long double __x);

float fmodf(float __x, float __y);
double fmod(double __x, double __y);
long double fmodl(long double __x, long double __y);

float modff(float __x, float* __iptr);
double modf(double __x, double* __iptr);
long double modfl(long double __x, long double* __iptr);

float fminf(float __x, float __y);
double fmin(double __x, double __y);
long double fminl(long double __x, long double __y);

float fmaxf(float __x, float __y);
double fmax(double __x, double __y);
long double fmaxl(long double __x, long double __y);

float fmaf(float __x, float __y, float __z);
double fma(double __x, double __y, double __z);
long double fmal(long double __x, long double __y, long double __z);

float sinf(float __x);
double sin(double __x);
long double sinl(long double __x);

float cosf(float __x);
double cos(double __x);
long double cosl(long double __x);

float tanf(float __x);
double tan(double __x);
long double tanl(long double __x);

void sincosf(float __x, float* __sin, float* __cos);
void sincos(double __x, double* __sin, double* __cos);
void sincosl(long double __x, long double* __sin, long double* __cos);

float hypotf(float __x, float __y);
double hypot(double __x, double __y);
long double hypotl(long double __x, long double __y);

float sinhf(float __x);
double sinh(double __x);
long double sinhl(long double __x);

float coshf(float __x);
double cosh(double __x);
long double coshl(long double __x);

float tanhf(float __x);
double tanh(double __x);
long double tanhl(long double __x);

float asinf(float __x);
double asin(double __x);
long double asinl(long double __x);

float acosf(float __x);
double acos(double __x);
long double acosl(long double __x);

float atanf(float __x);
double atan(double __x);
long double atanl(long double __x);

float atan2f(float __y, float __x);
double atan2(double __y, double __x);
long double atan2l(long double __y, long double __x);

float asinhf(float __x);
double asinh(double __x);
long double asinhl(long double __x);

float acoshf(float __x);
double acosh(double __x);
long double acoshl(long double __x);

float atanhf(float __x);
double atanh(double __x);
long double atanhl(long double __x);

float floorf(float __x);
double floor(double __x);
long double floorl(long double __x);

float ceilf(float __x);
double ceil(double __x);
long double ceill(long double __x);

float roundf(float __x);
double round(double __x);
long double roundl(long double __x);

float fabsf(float __x);
double fabs(double __x);
long double fabsl(long double __x);

float fdimf(float __x, float __y);
double fdim(double __x, double __y);
long double fdiml(long double __x, long double __y);

float remainderf(float __x, float __y);
double remainder(double __x, double __y);
long double remainderl(long double __x, long double __y);

float remquof(float __x, float __y, int* __quot);
double remquo(double __x, double __y, int* __quot);
long double remquol(long double __x, long double __y, int* __quot);

float copysignf(float __x, float __y);
double copysign(double __x, double __y);
long double copysignl(long double __x, long double __y);

float erff(float __x);
double erf(double __x);
long double erfl(long double __x);

float erfcf(float __x);
double erfc(double __x);
long double erfcl(long double __x);

float nearbyintf(float __x);
double nearbyint(double __x);
long double nearbyintl(long double __x);

float scalbnf(float __x, int __exp);
double scalbn(double __x, int __exp);
long double scalbnl(long double __x, int __exp);

float scalblnf(float __x, long __exp);
double scalbln(double __x, long __exp);
long double scalblnl(long double __x, long __exp);

float frexpf(float __x, int* __exp);
double frexp(double __x, int* __exp);
long double frexpl(long double __x, int* __exp);

float ldexpf(float __x, int __exp);
double ldexp(double __x, int __exp);
long double ldexpl(long double __x, int __exp);

float lgammaf(float __x);
double lgamma(double __x);
long double lgammal(long double __x);

float rintf(float __x);
double rint(double __x);
long double rintl(long double __x);

float tgammaf(float __x);
double tgamma(double __x);
long double tgammal(long double __x);

float truncf(float __x);
double trunc(double __x);
long double truncl(long double __c);

long lrintf(float __x);
long lrint(double __x);
long lrintl(long double __x);

long long llrintf(float __x);
long long llrint(double __x);
long long llrintl(long double __x);

long lroundf(float __x);
long lround(double __x);
long lroundl(long double __x);

long long llroundf(float __x);
long long llround(double __x);
long long llroundl(long double __x);

int ilogbf(float __x);
int ilogb(double __x);
int ilogbl(long double __x);

float nanf(const char* __str);
double nan(const char* __str);
long double nanl(const char* __str);

float nextafterf(float __from, float __to);
double nextafter(double __from, double __to);
long double nextafterl(long double __from, long double __to);

float nexttowardf(float __from, long double __to);
double nexttoward(double __from, long double __to);
long double nexttowardl(long double __from, long double __to);

extern int signgam;

float j0f(float __x);
double j0(double __x);
float j1f(float __x);
double j1(double __x);
float jnf(int __n, float __x);
double jn(int __n, double __x);

float y0f(float __x);
double y0(double __x);
float y1f(float __x);
double y1(double __x);
float ynf(int __n, float __x);
double yn(int __n, double __x);

float dremf(float __x, float __y);
double drem(double __x, double __y);

int finitef(float __x);
int finite(double __x);

float scalbf(float __x, float __y);
double scalb(double __x, double __y);

float significandf(float __x);
double significand(double __x);

float lgammaf_r(float __x, int* __sigptr);
double lgamma_r(double __x, int* __sigptr);
long double lgammal_r(long double __x, int* __sigptr);

__end_decls

#endif
