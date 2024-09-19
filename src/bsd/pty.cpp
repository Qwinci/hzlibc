#include "pty.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"
#include "unistd.h"

EXPORT int openpty(int* mfd, int* sfd, char* name, const termios* term, const winsize* win) {
	int fd;
	if (auto err = sys_openat(AT_FDCWD, "/dev/ptmx", O_RDWR | O_NOCTTY, 0, &fd)) {
		errno = err;
		return -1;
	}

	char spath[32];
	if (!name) {
		name = spath;
	}
	if (auto err = sys_ptsname(fd, name, 32)) {
		errno = err;
		sys_close(fd);
		return -1;
	}

	int pts_fd;
	unlockpt(fd);
	if (auto err = sys_openat(AT_FDCWD, name, O_RDWR | O_NOCTTY, 0, &pts_fd)) {
		errno = err;
		sys_close(fd);
		return -1;
	}

	if (term) {
		tcsetattr(fd, TCSAFLUSH, term);
	}

	if (win) {
		ioctl(fd, TIOCSWINSZ, win);
	}

	*mfd = fd;
	*sfd = pts_fd;
	return 0;
}
