#ifndef _SYS_TTYDEFAULTS_H
#define _SYS_TTYDEFAULTS_H

#define CTRL(x) (x & 037)
#define CEOF CTRL('d')
#define CERASE 0177
#define CINTR CTRL('c')
#define CKILL CTRL('u')
#define CQUIT 034
#define CSUSP CTRL('z')
#define CSTART CTRL('q')
#define CSTOP CTRL('s')
#define CLNEXT CTRL('v')
#define CWERASE CTRL('w')

#endif
