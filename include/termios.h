#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <bits/termios.h>

#define CS5 0
#define CS6 0x10
#define CS7 0x20
#define CS8 0x30
#define CSIZE 0x30
#define CSTOPB 0x40
#define CREAD 0x80
#define PARENB 0x100
#define PARODD 0x200
#define HUPCL 0x400
#define CLOCAL 0x800
#define CBAUD 0x100F

#define ISIG 1
#define ICANON 2
#define XCASE 4
#define ECHO 8
#define ECHOE 0x10
#define ECHOK 0x20
#define ECHONL 0x40
#define NOFLSH 0x80
#define TOSTOP 0x100
#define ECHOCTL 0x200
#define ECHOPRT 0x400
#define ECHOKE 0x800
#define FLUSHO 0x1000
#define PENDIN 0x4000
#define IEXTEN 0x8000
#define EXTPROC 0x10000

#define IGNBRK 1
#define BRKINT 2
#define IGNPAR 4
#define PARMRK 8
#define INPCK 0x10
#define ISTRIP 0x20
#define INLCR 0x40
#define IGNCR 0x80
#define ICRNL 0x100
#define IUCLC 0x200
#define IXON 0x400
#define IXANY 0x800
#define IXOFF 0x1000
#define IMAXBEL 0x2000
#define IUTF8 0x4000

#define OPOST 1
#define OLCUC 2
#define ONLCR 4
#define OCRNL 8
#define ONOCR 0x10
#define ONLRET 0x20
#define OFILL 0x40
#define OFDEL 0x80
#define NLDLY 0x100
#define NL0 0
#define NL1 0x100
#define CRDLY 0x600
#define CR0 0
#define CR1 0x200
#define CR2 0x400
#define CR3 0x600
#define TABDLY 0x1800
#define TAB0 0
#define TAB1 0x800
#define TAB2 0x1000
#define TAB3 0x1800
#define XTABS TAB3
#define BSDLY 0x2000
#define BS0 0
#define BS1 0x2000
#define VTDLY 0x4000
#define VT0 0
#define VT1 0x4000
#define FFDLY 0x8000
#define FF0 0
#define FF1 0x8000

#define B0 0
#define B50 1
#define B75 2
#define B110 3
#define B134 4
#define B150 5
#define B200 6
#define B300 7
#define B600 8
#define B1200 9
#define B1800 0xA
#define B2400 0xB
#define B4800 0xC
#define B9600 0xD
#define B19200 0xE
#define B38400 0xF
#define EXTA B19200
#define EXTB B38400

#define TIOCSER_TEMT 1

#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

#define	TCSANOW 0
#define	TCSADRAIN 1
#define	TCSAFLUSH 2

#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VTIME 5
#define VMIN 6
#define VSWTC 7
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define VEOL 11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

#endif
