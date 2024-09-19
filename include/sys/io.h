#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <bits/utils.h>

__begin_decls

__attribute__((deprecated)) int iopl(int __level);

static __inline unsigned char inb(unsigned short __port) {
	unsigned char __value;
	__asm__ __volatile__("inb %w1, %0" : "=a"(__value) : "Nd"(__port));
	return __value;
}

static __inline unsigned short inw(unsigned short __port) {
	unsigned short __value;
	__asm__ __volatile__("inw %w1, %0" : "=a"(__value) : "Nd"(__port));
	return __value;
}

static __inline unsigned int inl(unsigned short __port) {
	unsigned int __value;
	__asm__ __volatile__("inl %w1, %0" : "=a"(__value) : "Nd"(__port));
	return __value;
}

static __inline void outb(unsigned char __value, unsigned short __port) {
	__asm__ __volatile__("outb %b0, %w1" : : "a"(__value), "Nd"(__port));
}

static __inline void outw(unsigned short __value, unsigned short __port) {
	__asm__ __volatile__("outw %w0, %w1" : : "a"(__value), "Nd"(__port));
}

static __inline void outl(unsigned int __value, unsigned short __port) {
	__asm__ __volatile__("outl %0, %w1" : : "a"(__value), "Nd"(__port));
}

__end_decls

#endif
