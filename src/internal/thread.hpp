#pragma once

[[noreturn]] void hzlibc_thread_exit(void* ret);
extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg);
