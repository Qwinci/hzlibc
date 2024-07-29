#include "syslog.h"
#include "utils.hpp"
#include "mutex.hpp"
#include "sys/un.h"
#include "unistd.h"
#include "allocator.hpp"
#include "errno.h"
#include "stdio.h"
#include "time.h"
#include "fcntl.h"
#include <hz/string.hpp>

namespace {
	Mutex LOCK {};
	int LOG_FD = -1;
	constexpr sockaddr_un LOG_ADDR {AF_UNIX, "/dev/log"};
	hz::string<Allocator> IDENT {Allocator {}};
	int OPTIONS = 0;
	int FACILITY = 0;
	bool USE_LIBC_LOG = false;

	void open_log() {
		LOG_FD = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
		if (LOG_FD < 0) {
			return;
		}
		if (connect(LOG_FD, reinterpret_cast<const sockaddr*>(&LOG_ADDR), sizeof(LOG_ADDR))) {
			close(LOG_FD);
			LOG_FD = -1;
			USE_LIBC_LOG = true;
		}
	}
}

EXPORT void openlog(const char* ident, int options, int facility) {
	auto guard = LOCK.lock();
	if (ident) {
		IDENT = ident;
	}
	else {
		IDENT = program_invocation_short_name;
	}

	OPTIONS = options;
	FACILITY = facility;
	if ((options & LOG_NDELAY) && LOG_FD < 0) {
		open_log();
	}
}

EXPORT void closelog() {
	auto guard = LOCK.lock();
	if (LOG_FD != -1) {
		close(LOG_FD);
		LOG_FD = -1;
	}
}

EXPORT void vsyslog(int priority, const char* fmt, va_list ap) {
	char buf[1024];

	if (USE_LIBC_LOG) {
		int len = vsnprintf(buf, sizeof(buf), fmt, ap);
		hz::string_view str {buf, static_cast<size_t>(len)};
		if (!str.ends_with('\n')) {
			println("syslog: ", str);
		}
		else {
			print("syslog: ", str);
		}
		return;
	}

	int saved_errno = errno;

	if (LOG_FD < 0) {
		open_log();
	}

	if (!(priority & LOG_FACMASK)) {
		priority |= FACILITY;
	}

	time_t now = time(nullptr);
	tm t {};
	gmtime_r(&now, &t);
	char time_buf[32];
	strftime(time_buf, sizeof(time_buf), "%b %e %T", &t);

	pid_t pid = 0;
	if (OPTIONS & LOG_PID) {
		pid = getpid();
	}
	int len = snprintf(
		buf,
		sizeof(buf),
		"<%d>%s %s%s%.0d%s",
		priority,
		time_buf,
		IDENT.data(),
		(pid ? "[" : ""),
		pid,
		(pid ? "]" : ""));
	errno = saved_errno;
	int len2 = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);

	auto is_log_closed = [](int err) {
		return err == ECONNREFUSED || err == ECONNRESET || err == ENOTCONN || err == EPIPE;
	};

	if (len2 >= 0) {
		if (len2 >= static_cast<ssize_t>(sizeof(buf) - len)) {
			len = sizeof(buf) - 1;
		}
		else {
			len += len2;
		}
		if (buf[len - 1] != '\n') {
			buf[len++] = '\n';
		}
		if (send(LOG_FD, buf, static_cast<size_t>(len), 0) < 0 && (
			!is_log_closed(errno) ||
			connect(
				LOG_FD,
				reinterpret_cast<const sockaddr*>(&LOG_ADDR),
				sizeof(LOG_ADDR)) < 0 ||
			send(LOG_FD, buf, static_cast<size_t>(len), 0) < 0) &&
			(OPTIONS & LOG_CONS)) {
			int fd = open("/dev/console", O_WRONLY | O_NOCTTY | O_CLOEXEC);
			if (fd >= 0) {
				FILE* f = fdopen(fd, "w");
				if (f) {
					fputs(buf, f);
					fclose(f);
				}
				else {
					close(fd);
				}
			}

			if (OPTIONS & LOG_PERROR) {
				fputs(buf, stderr);
			}
		}
	}
}

EXPORT void syslog(int priority, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsyslog(priority, fmt, ap);
	va_end(ap);
}
