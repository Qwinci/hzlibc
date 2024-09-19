#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#define bswap_16(value) __builtin_bswap16(value)
#define bswap_32(value) __builtin_bswap32(value)
#define bswap_64(value) __builtin_bswap64(value)

#define __bswap_16(value) __builtin_bswap16(value)
#define __bswap_32(value) __builtin_bswap32(value)
#define __bswap_64(value) __builtin_bswap64(value)

#endif
