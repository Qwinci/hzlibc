#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <bits/utils.h>

__begin

int ioctl(int __fd, unsigned long __op, ...);

#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414

struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;
	unsigned short ws_ypixel;
};

__end

#endif
