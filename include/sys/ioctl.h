#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <bits/utils.h>

__begin_decls

#define _IOC_NONE 0U
#define _IOC_WRITE 1U
#define _IOC_READ 2U

#define _IOC(dir, type, num, size) (((dir) << 30) | ((size) << 16) | ((type) << 8) | (num))

#define _IO(type, num) _IOC(_IOC_NONE, (type), (num), 0)
#define _IOR(type, num, size) _IOC(_IOC_READ, (type), (num), sizeof(size))
#define _IOW(type, num, size) _IOC(_IOC_WRITE, (type), (num), sizeof(size))
#define _IOWR(type, num, size) _IOC(_IOC_READ | _IOC_WRITE, (type), (num), sizeof(size))

#define SIOCGIFFLAGS 0x8913
#define SIOCGIFBRDADDR 0x8919
#define SIOCGIFNETMASK 0x891B
#define SIOCGIFMTU 0x8921
#define SIOCGIFHWADDR 0x8927
#define SIOCGIFINDEX 0x8933

#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCSETSW 0x5403
#define TCSETSF 0x5404
#define TCSBRK 0x5409
#define TCXONC 0x540A
#define TCFLSH 0x540B
#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCLINUX 0x541C
#define TIOCSPTLCK 0x40045431
#define TIOCGPTN 0x80045430

#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

#define SIOCGIFCONF 0x8912

#ifndef FIONREAD
#define FIONREAD 0x541B
#endif
#define FIONBIO 0x5421

int ioctl(int __fd, unsigned long __op, ...);

__end_decls

#endif
