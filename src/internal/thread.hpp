#pragma once

extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg);
