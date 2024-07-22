#pragma once

inline int syscall_error(long ret) {
	if (static_cast<unsigned long>(ret) > -4096UL) {
		return static_cast<int>(-ret);
	}
	else {
		return 0;
	}
}

inline long syscall(long num) {
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0>
inline long syscall(long num, A0 a0) {
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0, typename A1>
inline long syscall(long num, A0 a0, A1 a1) {
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0),
		"S"((long) a1) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0, typename A1, typename A2>
inline long syscall(long num, A0 a0, A1 a1, A2 a2) {
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0),
		"S"((long) a1),
		"d"((long) a2) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3) {
	register long r10 asm("r10") = (long) a3;
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0),
		"S"((long) a1),
		"d"((long) a2),
		"r"(r10) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) {
	register long r10 asm("r10") = (long) a3;
	register long r8 asm("r8") = (long) a4;
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0),
		"S"((long) a1),
		"d"((long) a2),
		"r"(r10),
		"r"(r8) :
		"rcx", "r11", "memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
	register long r10 asm("r10") = (long) a3;
	register long r8 asm("r8") = (long) a4;
	register long r9 asm("r9") = (long) a5;
	long ret;
	asm volatile("syscall" :
		"=a"(ret) :
		"a"(num),
		"D"((long) a0),
		"S"((long) a1),
		"d"((long) a2),
		"r"(r10),
		"r"(r8),
		"r"(r9) :
		"rcx", "r11", "memory");
	return ret;
}
