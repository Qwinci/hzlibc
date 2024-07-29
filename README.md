# Horizon libc
Horizon libc (hzlibc) is a libc intented to be relatively straight-forward to port to different OS's and architectures.
It also tries to be one-way binary compatible with glibc so binaries linked against glibc should just work.

## Todo
- Installation of headers and libraries
- There is a lot of functions still to be implemented

## Acknowledgements
- [Mlibc](https://github.com/managarm/mlibc), there is some stuff heavily inspired by it.
- Rust Libstd, some pthread primitives are ported from it.
- llvm libunwind (thirdparty/libunwind) used for `backtrace` function.
- [libucontext](https://github.com/kaniini/libucontext), ucontext functions are based on it.
