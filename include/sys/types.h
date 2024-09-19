#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <bits/utils.h>
#include <bits/time_t.h>

__begin_decls

typedef long off_t;
typedef long ssize_t;
typedef uint32_t mode_t;
typedef int32_t pid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t id_t;
typedef uint64_t dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef long blksize_t;
typedef long blkcnt_t;
typedef unsigned long fsblkcnt_t;
typedef long __fsword_t;
typedef unsigned long __fsblkcnt_t;
typedef unsigned long __fsfilcnt_t;
typedef long suseconds_t;
typedef int32_t key_t;
typedef uint32_t useconds_t;

typedef int64_t off64_t;
typedef off64_t loff_t;
typedef uint64_t ino64_t;
typedef int64_t blkcnt64_t;
typedef uint64_t __fsblkcnt64_t;
typedef uint64_t __fsfilcnt64_t;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef char* caddr_t;

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

typedef struct {
	int __value[2];
} __fsid_t;

__end_decls

#endif
