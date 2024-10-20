# Horizon libc
Horizon libc (hzlibc) is a libc intended to be relatively straight-forward to port to different OS's and architectures.
It also tries to be one-way binary compatible with glibc so binaries linked against glibc should just work.

# Configuration
- CMake option `HEADERS_ONLY` can be used to only install headers
- CMake option `ANSI_ONLY` can be used to only build the ansi part and dynamic linker
- CMake option `ENABLE_LIBC_TESTING` can be used to enable libc testing
- CMake option `DEFAULT_LIBRARY_PATHS` is a semicolon-separated list of default library paths
- File `/etc/hzlibc_ld.conf` can be used to add additional library search paths one per line

# Porting
There is a template folder in sys/ that can be used as a starting point by copying + renaming it and adding
a check for your system in sys/CMakeLists.txt.

## Todo
- Static linking
- There are a lot of functions still to be implemented

## Acknowledgements
- [Mlibc](https://github.com/managarm/mlibc), there is some stuff heavily inspired by it.
- Rust Libstd, some pthread primitives are ported from it.
- Musl, math functions are from it.
- llvm libunwind (thirdparty/libunwind) used for `backtrace` function.
- llvm compiler-rt, some compiler support functions (in src/internal/float_gcc.cpp) are from it.
- [libucontext](https://github.com/kaniini/libucontext), ucontext functions are based on it.
