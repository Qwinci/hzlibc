#ifndef _PTY_H
#define _PTY_H

#include <bits/utils.h>
#include <termios.h>
#include <sys/ioctl.h>

__begin_decls

int openpty(int* __mfd, int* __sfd, char* __name, const struct termios* __term, const struct winsize* __win);

__end_decls

#endif
