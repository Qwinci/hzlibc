#ifndef _BITS_THREAD_TYPES_H
#define _BITS_THREAD_TYPES_H

typedef unsigned long __hzlibc_thread_t;
typedef unsigned int __hzlibc_key_t;
typedef int __hzlibc_once_t;
typedef volatile int __hzlibc_spinlock_t;

enum {
	__HZLIBC_MUTEX_NORMAL,
	__HZLIBC_MUTEX_RECURSIVE,
	__HZLIBC_MUTEX_ERRORCHECK
};

#define __HZLIBC_ONCE_INIT 0
#define __HZLIBC_MUTEX_INITIALIZER {{0}}
#define __HZLIBC_COND_INITIALIZER {0}

#define __HZLIBC_CANCEL_ENABLE 0
#define __HZLIBC_CANCEL_DISABLE 1

#define __HZLIBC_CANCEL_DEFERRED 0
#define __HZLIBC_CANCEL_ASYNCHRONOUS 1

#define __HZLIBC_THREAD_CREATE_JOINABLE 0
#define __HZLIBC_THREAD_CREATE_DETACHED 1

#define __HZLIBC_THREAD_STACK_MIN 16384

#define __HZLIBC_RWLOCK_INITIALIZER {0}

#define __HZLIBC_BARRIER_SERIAL_THREAD -1

typedef union {
#ifdef __x86_64__
	char __size[40];
#elif defined(__i386__)
	char __size[24];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_mutex_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[48];
#else
#error missing architecture specific code
#endif
	long long __align;
} __hzlibc_cond_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[32];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_rwlock_t;

typedef union {
#ifdef __x86_64__
	char __size[32];
#elif defined(__i386__)
	char __size[20];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_barrier_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} __hzlibc_mutexattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} __hzlibc_condattr_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[36];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_attr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[8];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_rwlockattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	long __align;
} __hzlibc_barrierattr_t;

#endif
