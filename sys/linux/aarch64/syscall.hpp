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
	register long x8 asm("x8") = num;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8) :
		"memory");
	return ret;
}

template<typename A0>
inline long syscall(long num, A0 a0) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0) :
		"memory");
	return ret;
}

template<typename A0, typename A1>
inline long syscall(long num, A0 a0, A1 a1) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long x1 asm("x1") = (long) a1;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0),
		"r"(x1) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2>
inline long syscall(long num, A0 a0, A1 a1, A2 a2) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long x1 asm("x1") = (long) a1;
	register long x2 asm("x2") = (long) a2;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0),
		"r"(x1),
		"r"(x2) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long x1 asm("x1") = (long) a1;
	register long x2 asm("x2") = (long) a2;
	register long x3 asm("x3") = (long) a3;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0),
		"r"(x1),
		"r"(x2),
		"r"(x3) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long x1 asm("x1") = (long) a1;
	register long x2 asm("x2") = (long) a2;
	register long x3 asm("x3") = (long) a3;
	register long x4 asm("x4") = (long) a4;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0),
		"r"(x1),
		"r"(x2),
		"r"(x3),
		"r"(x4) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
	register long x8 asm("x8") = num;
	register long x0 asm("x0") = (long) a0;
	register long x1 asm("x1") = (long) a1;
	register long x2 asm("x2") = (long) a2;
	register long x3 asm("x3") = (long) a3;
	register long x4 asm("x4") = (long) a4;
	register long x5 asm("x5") = (long) a5;
	register long ret asm("x0");
	asm volatile("svc #0" :
		"=r"(ret) :
		"r"(x8),
		"r"(x0),
		"r"(x1),
		"r"(x2),
		"r"(x3),
		"r"(x4),
		"r"(x5) :
		"memory");
	return ret;
}
