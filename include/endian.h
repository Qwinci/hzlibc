#ifndef _ENDIAN_H
#define _ENDIAN_H

#include <stdint.h>

#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#define BYTE_ORDER __BYTE_ORDER__

#if BYTE_ORDER == LITTLE_ENDIAN

#define le16toh(value) (uint16_t) (value)
#define le32toh(value) (uint32_t) (value)
#define le64toh(value) (uint64_t) (value)

#define be16toh(value) __builtin_bswap16(value)
#define be32toh(value) __builtin_bswap32(value)
#define be64toh(value) __builtin_bswap64(value)

#define htole16(value) (uint16_t) (value)
#define htole32(value) (uint32_t) (value)
#define htole64(value) (uint64_t) (value)

#define htobe16(value) __builtin_bswap16(value)
#define htobe32(value) __builtin_bswap32(value)
#define htobe64(value) __builtin_bswap64(value)

#else

#define le16toh(value) __builtin_bswap16(value)
#define le32toh(value) __builtin_bswap16(value)
#define le64toh(value) __builtin_bswap64(value)

#define be16toh(value) (uint16_t) (value)
#define be32toh(value) (uint32_t) (value)
#define be64toh(value) (uint64_t) (value)

#define htole16(value) __builtin_bswap16(value)
#define htole32(value) __builtin_bswap16(value)
#define htole64(value) __builtin_bswap64(value)

#define htobe16(value) (uint16_t) (value)
#define htobe32(value) (uint32_t) (value)
#define htobe64(value) (uint64_t) (value)

#endif

#endif
