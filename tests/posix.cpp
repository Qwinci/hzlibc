#include <gtest/gtest.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

TEST(posix, fnmatch) {
	EXPECT_EQ(fnmatch("abc", "abc", 0), 0);
	EXPECT_EQ(fnmatch("abc", "abd", 0), FNM_NOMATCH);

	EXPECT_EQ(fnmatch("abc", "aBc", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("abc", "aBc", FNM_CASEFOLD), 0);

	EXPECT_EQ(fnmatch("ab[cd]", "abc", 0), 0);
	EXPECT_EQ(fnmatch("ab[cd]", "abd", 0), 0);
	EXPECT_EQ(fnmatch("ab[cd]", "abe", 0), FNM_NOMATCH);

	// ^ should be allowed unless POSIXLY_CORRECT env is set
	EXPECT_EQ(fnmatch("ab[^cd]", "abc", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab[^cd]", "abd", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab[^cd]", "abe", 0), 0);

	EXPECT_EQ(fnmatch("ab[!cd]", "abc", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab[!cd]", "abd", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab[!cd]", "abe", 0), 0);

	EXPECT_EQ(fnmatch("a[0-9]b", "a0b", 0), 0);
	EXPECT_EQ(fnmatch("a[0-9]b", "aOb", 0), FNM_NOMATCH);

	// ^ should be allowed unless POSIXLY_CORRECT env is set
	EXPECT_EQ(fnmatch("a[^0-9]b", "aOb", 0), 0);
	EXPECT_EQ(fnmatch("a[^0-9]b", "a0b", 0), FNM_NOMATCH);

	EXPECT_EQ(fnmatch("a[!0-9]b", "aOb", 0), 0);
	EXPECT_EQ(fnmatch("a[!0-9]b", "a0b", 0), FNM_NOMATCH);

	EXPECT_EQ(fnmatch("a[[:digit:]]b", "a0b", 0), 0);
	EXPECT_EQ(fnmatch("a[[:digit:]]b", "aOb", 0), FNM_NOMATCH);

	// ^ should be allowed unless POSIXLY_CORRECT env is set
	EXPECT_EQ(fnmatch("a[^[:digit:]]b", "a0b", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("a[^[:digit:]]b", "aOb", 0), 0);

	EXPECT_EQ(fnmatch("a[![:digit:]]b", "a0b", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("a[![:digit:]]b", "aOb", 0), 0);

	EXPECT_EQ(fnmatch("*.html", "abc.html", 0), 0);
	EXPECT_EQ(fnmatch("*.html", "abc.txt", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab*", "ab", 0), 0);
	EXPECT_EQ(fnmatch("ab*", "abc", 0), 0);
	EXPECT_EQ(fnmatch("ab*", "abcd", 0), 0);
	EXPECT_EQ(fnmatch("ab*", "ab/cd", 0), 0);
	EXPECT_EQ(fnmatch("ab*", "ab/cd", FNM_PATHNAME), FNM_NOMATCH);

	EXPECT_EQ(fnmatch("ab?", "ab", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab?", "abc", 0), 0);
	EXPECT_EQ(fnmatch("ab?", "abcd", 0), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab?d", "abcd", 0), 0);
	EXPECT_EQ(fnmatch("ab?cd", "ab/cd", 0), 0);
	EXPECT_EQ(fnmatch("ab?cd", "ab/cd", FNM_PATHNAME), FNM_NOMATCH);

	EXPECT_EQ(fnmatch("*ab", "1/2/3/4/ab/cd", FNM_PATHNAME | FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab*", "ab/cd/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab*", "ab/cd/ef", FNM_PATHNAME | FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("*ab*", "1/2/3/4/ab/cd/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("*ab*", "1/2/3/4/ab/cd/ef", FNM_PATHNAME | FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab?", "ab/cd/ef", FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab?", "abx/cd/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab/", "ab/cd/ef", FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab/*", "ab/cd/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab/?", "ab/cd/ef", FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab*c", "ab/1/2/3/c/d/e", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab?c", "ab/c/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab*c*", "ab/1/2/3/c/d/e", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab?c*", "ab/c/ef", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab*c/", "ab/1/2/3/c/d/e", FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab?c/", "ab/c/ef", FNM_LEADING_DIR), FNM_NOMATCH);
	EXPECT_EQ(fnmatch("ab*c/*", "ab/1/2/3/c/d/e", FNM_LEADING_DIR), 0);
	EXPECT_EQ(fnmatch("ab?c/*", "ab/c/ef", FNM_LEADING_DIR), 0);
}

TEST(posix, realpath) {
	char* name = realpath("/bin", nullptr);
	EXPECT_NE(name, nullptr);
	EXPECT_EQ(strcmp(name, "/usr/bin"), 0);
	free(name);
	name = realpath("/bin/", nullptr);
	EXPECT_NE(name, nullptr);
	EXPECT_EQ(strcmp(name, "/usr/bin"), 0);
	free(name);

	name = realpath("//", nullptr);
	EXPECT_NE(name, nullptr);
	EXPECT_EQ(strcmp(name, "/"), 0);
	free(name);
}

static volatile int SIGINT_OCCURRED = 0;

static void sigint_handler(int sig) {
	SIGINT_OCCURRED += 1;
}

TEST(posix, signals) {
	pid_t pid = getpid();
	signal(SIGINT, sigint_handler);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	EXPECT_EQ(pthread_sigmask(SIG_BLOCK, &set, nullptr), 0);
	char command[80];
	snprintf(command, 80, "sh -c 'sleep 1; kill -INT %ld' &", static_cast<long>(pid));
	EXPECT_EQ(system(command), 0);
	sleep(2);
	EXPECT_EQ(SIGINT_OCCURRED, 0);
	EXPECT_EQ(pthread_sigmask(SIG_UNBLOCK, &set, nullptr), 0);
	EXPECT_EQ(SIGINT_OCCURRED, 1);
}

TEST(posix, semaphore) {
	sem_t s {};
	EXPECT_EQ(sem_init(&s, 0, 0), 0);
	EXPECT_EQ(sem_init(&s, 0, 1), 0);
	EXPECT_EQ(sem_init(&s, 0, 123), 0);

	errno = 0;
	EXPECT_EQ(sem_init(&s, 0, -1), -1);
	EXPECT_EQ(errno, EINVAL);

	EXPECT_EQ(sem_init(&s, 0, SEM_VALUE_MAX), 0);
	EXPECT_EQ(sem_init(&s, 0, static_cast<unsigned int>(SEM_VALUE_MAX) + 1), -1);
	EXPECT_EQ(sem_destroy(&s), 0);

	EXPECT_EQ(sem_init(&s, 0, 3), 0);
	EXPECT_EQ(sem_trywait(&s), 0);
	EXPECT_EQ(sem_trywait(&s), 0);
	EXPECT_EQ(sem_trywait(&s), 0);
	errno = 0;
	EXPECT_EQ(sem_trywait(&s), -1);
	EXPECT_EQ(errno, EAGAIN);
	EXPECT_EQ(sem_destroy(&s), 0);
}
