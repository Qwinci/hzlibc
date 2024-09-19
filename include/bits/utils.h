#ifndef _BITS_UTILS_H
#define _BITS_UTILS_H

#ifdef __cplusplus
#define __begin_decls extern "C" {
#define __end_decls }
#define __nothrow noexcept
#else
#define __begin_decls
#define __end_decls
#define __nothrow __attribute__((__nothrow__))
#endif

#endif
