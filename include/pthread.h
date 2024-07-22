#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <bits/utils.h>

__begin

typedef unsigned long pthread_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
typedef volatile int pthread_spinlock_t;

enum {
	PTHREAD_MUTEX_NORMAL,
	PTHREAD_MUTEX_RECURSIVE,
	PTHREAD_MUTEX_ERRORCHECK
};

typedef union {
#ifdef __x86_64__
	char __size[40];
#elif defined(__i386__)
	char __size[24];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_mutex_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[48];
#else
#error missing architecture specific code
#endif
	long long __align;
} pthread_cond_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[32];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_rwlock_t;

typedef union {
#ifdef __x86_64__
	char __size[32];
#elif defined(__i386__)
	char __size[20];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_barrier_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} pthread_mutexattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} pthread_condattr_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[36];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_attr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[8];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_rwlockattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_barrierattr_t;

int pthread_create(
	pthread_t* __restrict __thrd,
	const pthread_attr_t* __restrict __attr,
	void* (*__start_fn)(void* __arg),
	void* __restrict __arg);
int pthread_join(pthread_t __thrd, void** __ret);

int pthread_key_create(pthread_key_t* __key, void (*__destructor)(void* __arg));
int pthread_key_delete(pthread_key_t __key);

__end

#endif
