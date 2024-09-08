# Horizon libc
Horizon libc (hzlibc) is a libc intented to be relatively straight-forward to port to different OS's and architectures.
It also tries to be one-way binary compatible with glibc so binaries linked against glibc should just work.

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
- [libucontext](https://github.com/kaniini/libucontext), ucontext functions are based on it.
