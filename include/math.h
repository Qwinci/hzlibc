#ifndef _MATH_H
#define _MATH_H

#include <bits/utils.h>

__begin

#define HUGE_VALF __builtin_hugevalf()
#define HUGE_VAL __builtin_hugeval()
#define HUGE_VALL __builtin_hugevall()

#define INFINITY __builtin_inff()

#define isinf(value) __builtin_isinf(value)
#define isnan(value) __builtin_isnan(value)

float powf(float __x, float __y);
double pow(double __x, double __y);

float sqrtf(float __x);
double sqrt(double __x);

float logf(float __x);
double log(double __x);
long double logl(long double __x);

float log2f(float __x);
double log2(double __x);

float log10f(float __x);
double log10(double __x);

double cbrt(double __x);

float expf(float __x);
double exp(double __x);

float exp2f(float __x);
double exp2(double __x);

float fmodf(float __x, float __y);
double fmod(double __x, double __y);
long double fmodl(long double __x, long double __y);

float modff(float __x, float* __iptr);
double modf(double __x, double* __iptr);

float fminf(float __x, float __y);
double fmin(double __x, double __y);

float fmaxf(float __x, float __y);
double fmax(double __x, double __y);

float fmaf(float __x, float __y, float __z);
double fma(double __x, double __y, double __z);

float sinf(float __x);
double sin(double __x);

float cosf(float __x);
double cos(double __x);

double tan(double __x);

void sincosf(float __x, float* __sin, float* __cos);
void sincos(double __x, double* __sin, double* __cos);

float hypotf(float __x, float __y);

double sinh(double __x);

double cosh(double __x);

float tanhf(float __x);
double tanh(double __x);

double asin(double __x);

float acosf(float __x);
double acos(double __x);

float atanf(float __x);
double atan(double __x);

float atan2f(float __y, float __x);
double atan2(double __y, double __x);

float floorf(float __x);
double floor(double __x);

float ceilf(float __x);
double ceil(double __x);

float roundf(float __x);
double round(double __x);

float copysignf(float __x, float __y);
double copysign(double __x, double __y);

double scalbn(double __x, int __exp);

float frexpf(float __x, int* __exp);
double frexp(double __x, int* __exp);
long double frexpl(long double __x, int* __exp);

float ldexpf(float __x, int __exp);
double ldexp(double __x, int __exp);

long lroundf(float __x);
long lround(double __x);

float rintf(float __x);
double rint(double __x);

float truncf(float __x);
double trunc(double __x);

long lrintf(float __x);
long lrint(double __x);
long long llrint(double __x);

float nextafterf(float __from, float __to);
double nextafter(double __from, double __to);

__end

#endif
