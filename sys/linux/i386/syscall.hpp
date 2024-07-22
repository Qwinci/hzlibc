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
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num) :
		"memory");
	return ret;
}

template<typename A0>
inline long syscall(long num, A0 a0) {
	long ret;
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0) :
		"memory");
	return ret;
}

template<typename A0, typename A1>
inline long syscall(long num, A0 a0, A1 a1) {
	long ret;
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0),
		"c"((long) a1) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2>
inline long syscall(long num, A0 a0, A1 a1, A2 a2) {
	long ret;
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0),
		"c"((long) a1),
		"d"((long) a2) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3) {
	long ret;
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0),
		"c"((long) a1),
		"d"((long) a2),
		"S"((long) a3) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) {
	long ret;
	asm volatile("int $0x80" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0),
		"c"((long) a1),
		"d"((long) a2),
		"S"((long) a3),
		"D"((long) a4) :
		"memory");
	return ret;
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
inline long syscall(long num, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
	long ret;
	asm volatile(
		"push %%ebp;"
		"mov %0, %%ebp;"
		"int $0x80;"
		"pop %%ebp" :
		"=a"(ret) :
		"a"(num),
		"b"((long) a0),
		"c"((long) a1),
		"d"((long) a2),
		"S"((long) a3),
		"D"((long) a4),
		"rm"((long) a5) :
		"memory");
	return ret;
}
