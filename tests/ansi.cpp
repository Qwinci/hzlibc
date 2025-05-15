#include <gtest/gtest.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <setjmp.h>
#include <math.h>

TEST(ansi, ctype) {
	EXPECT_EQ(toupper('a'), 'A');
	EXPECT_EQ(toupper('A'), 'A');
	EXPECT_EQ(toupper('0'), '0');
	EXPECT_EQ(toupper(0), 0);

	EXPECT_EQ(tolower('a'), 'a');
	EXPECT_EQ(tolower('A'), 'a');
	EXPECT_EQ(tolower('0'), '0');
	EXPECT_EQ(tolower(0), 0);

	EXPECT_FALSE(isupper('a'));
	EXPECT_TRUE(isupper('A'));
	EXPECT_FALSE(isupper('0'));
	EXPECT_FALSE(isupper(0));

	EXPECT_TRUE(islower('a'));
	EXPECT_FALSE(islower('A'));
	EXPECT_FALSE(islower('0'));
	EXPECT_FALSE(islower(0));

	EXPECT_TRUE(isalpha('a'));
	EXPECT_TRUE(isalpha('A'));
	EXPECT_FALSE(isalpha('0'));
	EXPECT_FALSE(isalpha(0));

	EXPECT_FALSE(isdigit('a'));
	EXPECT_FALSE(isdigit('A'));
	EXPECT_TRUE(isdigit('0'));
	EXPECT_FALSE(isdigit(0));

	EXPECT_TRUE(isxdigit('a'));
	EXPECT_TRUE(isxdigit('A'));
	EXPECT_TRUE(isxdigit('0'));
	EXPECT_FALSE(isxdigit('g'));
	EXPECT_FALSE(isxdigit(0));

	EXPECT_FALSE(isspace('a'));
	EXPECT_FALSE(isspace('A'));
	EXPECT_FALSE(isspace('0'));
	EXPECT_FALSE(isspace(0));
	EXPECT_TRUE(isspace(' '));
	EXPECT_TRUE(isspace('\t'));
	EXPECT_TRUE(isspace('\n'));
	EXPECT_TRUE(isspace('\f'));

	EXPECT_TRUE(isprint('a'));
	EXPECT_TRUE(isprint('A'));
	EXPECT_TRUE(isprint('0'));
	EXPECT_FALSE(isprint(0));
	EXPECT_TRUE(isprint(' '));
	EXPECT_FALSE(isprint('\t'));
	EXPECT_FALSE(isprint('\n'));
	EXPECT_FALSE(isprint('\f'));

	EXPECT_FALSE(ispunct('a'));
	EXPECT_FALSE(ispunct('0'));
	EXPECT_TRUE(ispunct('.'));
	EXPECT_TRUE(ispunct(','));
	EXPECT_TRUE(ispunct(':'));
	EXPECT_TRUE(ispunct(';'));
	EXPECT_TRUE(ispunct('!'));
	EXPECT_TRUE(ispunct('@'));
	EXPECT_TRUE(ispunct('$'));

	EXPECT_TRUE(isalnum('a'));
	EXPECT_TRUE(isalnum('A'));
	EXPECT_TRUE(isalnum('0'));
	EXPECT_FALSE(isalnum('.'));
	EXPECT_FALSE(isalnum(','));
	EXPECT_FALSE(isalnum(':'));
	EXPECT_FALSE(isalnum(';'));
	EXPECT_FALSE(isalnum('!'));
	EXPECT_FALSE(isalnum('@'));
	EXPECT_FALSE(isalnum('$'));
}

void print_escaped_str(const char* str) {
	for (; *str; ++str) {
		switch (*str) {
			case '\t':
				fputs("\\t", stdout);
				break;
			case '\n':
				fputs("\\n", stdout);
				break;
			case '\f':
				fputs("\\f", stdout);
				break;
			default:
				fputc(*str, stdout);
				break;
		}
	}

	fputc('\n', stdout);
}

#define TEST_STR_INT(name) \
	str = " \t\n\f"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 0); \
	EXPECT_EQ(end, str); \
	str = " \t\n\f12"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 12); \
	EXPECT_EQ(end, str + 6); \
	str = "+12"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 12); \
	EXPECT_EQ(end, str + 3); \
	str = "-12"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), -12); \
	EXPECT_EQ(end, str + 3); \
	str = "0b1011"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 0b1011); \
	EXPECT_EQ(end, str + 6); \
	str = "0B1011"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 0b1011); \
	EXPECT_EQ(end, str + 6); \
	str = "01234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 01234); \
	EXPECT_EQ(end, str + 5); \
	str = "0x1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 0x1234); \
	EXPECT_EQ(end, str + 6); \
	str = "0X1234Af"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 0), 0x1234AF); \
	EXPECT_EQ(end, str + 8); \
	str = "0b1011"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 2), 0b1011); \
	EXPECT_EQ(end, str + 6); \
	str = "0B1011"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 2), 0b1011); \
	EXPECT_EQ(end, str + 6); \
	str = "1011"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 2), 0b1011); \
	EXPECT_EQ(end, str + 4); \
	str = "1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 8), 01234); \
	EXPECT_EQ(end, str + 4); \
	str = "0x1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 16), 0x1234); \
	EXPECT_EQ(end, str + 6); \
	str = "0X1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 16), 0x1234); \
	EXPECT_EQ(end, str + 6); \
	str = "1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, &end, 16), 0x1234); \
	EXPECT_EQ(end, str + 4); \
	\
	str = "1234"; \
	print_escaped_str(str); \
	EXPECT_EQ(name(str, nullptr, 0), 1234)

TEST(ansi, inttypes) {
	char* end;
	const char* str;

	TEST_STR_INT(strtoumax);
	TEST_STR_INT(strtoimax);

	EXPECT_EQ(imaxabs(1), 1);
	EXPECT_EQ(imaxabs(-1), 1);

	auto res = imaxdiv(1235, 2);
	assert(res.quot == 617);
	assert(res.rem == 1);
}

TEST(ansi, setjmp) {
	jmp_buf buf {};

	int count = 0;

	auto res = setjmp(buf);
	if (count == 0) {
		EXPECT_EQ(res, 0);
	}
	else if (count == 3) {
		return;
	}
	else {
		EXPECT_EQ(res, count);
		++count;
	}

	++count;
	longjmp(buf, count);
	EXPECT_TRUE(false);
}

static int SIGNAL_COUNT = 0;

TEST(ansi, signal) {
	auto old = signal(SIGALRM, [](int sig) {
		EXPECT_EQ(sig, SIGALRM);
		++SIGNAL_COUNT;
	});
	EXPECT_EQ(old, SIG_DFL);

	EXPECT_EQ(SIGNAL_COUNT, 0);
	EXPECT_EQ(raise(SIGALRM), 0);
	EXPECT_EQ(SIGNAL_COUNT, 1);

	signal(SIGALRM, SIG_IGN);

	SIGNAL_COUNT = 0;
	EXPECT_EQ(raise(SIGALRM), 0);
	EXPECT_EQ(SIGNAL_COUNT, 0);
}

TEST(ansi, printf) {
	char buf[128];
	EXPECT_EQ(snprintf(buf, 2, "a"), 1);
	EXPECT_STREQ(buf, "a");
	EXPECT_EQ(snprintf(buf, 3, "abc"), 3);
	EXPECT_STREQ(buf, "ab");
	EXPECT_EQ(snprintf(buf, 128, "%%"), 1);
	EXPECT_STREQ(buf, "%");
	EXPECT_EQ(snprintf(buf, 128, "a%cc%c", 'b', 'd'), 4);
	EXPECT_STREQ(buf, "abcd");
	EXPECT_EQ(snprintf(buf, 128, "a%lcc%lc", L'b', L'd'), 4);
	EXPECT_STREQ(buf, "abcd");
	EXPECT_EQ(snprintf(buf, 128, "%s", "hello"), 5);
	EXPECT_STREQ(buf, "hello");
	EXPECT_EQ(snprintf(buf, 128, "%ls", L"hello"), 5);
	EXPECT_STREQ(buf, "hello");

	EXPECT_EQ(snprintf(buf, 128, "%d", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%d", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%d", -1234), 5);
	EXPECT_STREQ(buf, "-1234");
	EXPECT_EQ(snprintf(buf, 128, "%hhd", 123), 3);
	EXPECT_STREQ(buf, "123");
	EXPECT_EQ(snprintf(buf, 128, "%hd", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%ld", 1234L), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%lld", 1234LL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%jd", intmax_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%zd", ssize_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%td", ptrdiff_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");

	EXPECT_EQ(snprintf(buf, 128, "%i", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%i", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%i", -1234), 5);
	EXPECT_STREQ(buf, "-1234");
	EXPECT_EQ(snprintf(buf, 128, "%hhi", 123), 3);
	EXPECT_STREQ(buf, "123");
	EXPECT_EQ(snprintf(buf, 128, "%hi", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%li", 1234L), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%lli", 1234LL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%ji", intmax_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%zi", ssize_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%ti", ptrdiff_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");

	EXPECT_EQ(snprintf(buf, 128, "%b", 0b1011), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%b", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%hhb", 0b1011), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%hb", 0b1011), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%lb", 0b1011UL), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%llb", 0b1011ULL), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%jb", uintmax_t {0b1011}), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%zb", size_t {0b1011}), 4);
	EXPECT_STREQ(buf, "1011");
	EXPECT_EQ(snprintf(buf, 128, "%tb", 0b1011UL), 4);
	EXPECT_STREQ(buf, "1011");

	EXPECT_EQ(snprintf(buf, 128, "%o", 01234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%o", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%o", -01234), 11);
	EXPECT_STREQ(buf, "37777776544");
	EXPECT_EQ(snprintf(buf, 128, "%hho", 012), 2);
	EXPECT_STREQ(buf, "12");
	EXPECT_EQ(snprintf(buf, 128, "%ho", 01234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%lo", 01234UL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%llo", 01234ULL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%jo", uintmax_t {01234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%zo", size_t {01234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%to", 01234UL), 4);
	EXPECT_STREQ(buf, "1234");

	EXPECT_EQ(snprintf(buf, 128, "%x", 0x1234AB), 6);
	EXPECT_STREQ(buf, "1234ab");
	EXPECT_EQ(snprintf(buf, 128, "%x", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%x", -0x1234AB), 8);
	EXPECT_STREQ(buf, "ffedcb55");
	EXPECT_EQ(snprintf(buf, 128, "%X", 0x1234AB), 6);
	EXPECT_STREQ(buf, "1234AB");
	EXPECT_EQ(snprintf(buf, 128, "%hhx", 0xAB), 2);
	EXPECT_STREQ(buf, "ab");
	EXPECT_EQ(snprintf(buf, 128, "%hx", 0x12AB), 4);
	EXPECT_STREQ(buf, "12ab");
	EXPECT_EQ(snprintf(buf, 128, "%lx", 0x1234ABUL), 6);
	EXPECT_STREQ(buf, "1234ab");
	EXPECT_EQ(snprintf(buf, 128, "%llx", 0x1234ABULL), 6);
	EXPECT_STREQ(buf, "1234ab");
	EXPECT_EQ(snprintf(buf, 128, "%jx", uintmax_t {0x1234AB}), 6);
	EXPECT_STREQ(buf, "1234ab");
	EXPECT_EQ(snprintf(buf, 128, "%zx", size_t {0x1234AB}), 6);
	EXPECT_STREQ(buf, "1234ab");
	EXPECT_EQ(snprintf(buf, 128, "%tx", 0x1234ABUL), 6);
	EXPECT_STREQ(buf, "1234ab");

	EXPECT_EQ(snprintf(buf, 128, "%u", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%u", 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%u", -1234), 10);
	EXPECT_STREQ(buf, "4294966062");
	EXPECT_EQ(snprintf(buf, 128, "%hhu", 123), 3);
	EXPECT_STREQ(buf, "123");
	EXPECT_EQ(snprintf(buf, 128, "%hu", 1234), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%lu", 1234UL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%llu", 1234ULL), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%ju", uintmax_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%zu", size_t {1234}), 4);
	EXPECT_STREQ(buf, "1234");
	EXPECT_EQ(snprintf(buf, 128, "%tu", 1234UL), 4);
	EXPECT_STREQ(buf, "1234");

	EXPECT_EQ(snprintf(buf, 128, "%f", 1234.1234567), 11);
	EXPECT_STREQ(buf, "1234.123457");
	EXPECT_EQ(snprintf(buf, 128, "%f", 0.0), 8);
	EXPECT_STREQ(buf, "0.000000");
	EXPECT_EQ(snprintf(buf, 128, "%f", -1234.123456), 12);
	EXPECT_STREQ(buf, "-1234.123456");
	EXPECT_EQ(snprintf(buf, 128, "%f", INFINITY), 3);
	EXPECT_STREQ(buf, "inf");
	EXPECT_EQ(snprintf(buf, 128, "%04f", INFINITY), 4);
	EXPECT_STREQ(buf, " inf");
	EXPECT_EQ(snprintf(buf, 128, "%-4f", INFINITY), 4);
	EXPECT_STREQ(buf, "inf ");
	EXPECT_EQ(snprintf(buf, 128, "%f", NAN), 3);
	EXPECT_STREQ(buf, "nan");
	EXPECT_EQ(snprintf(buf, 128, "%lf", 1234.123456), 11);
	EXPECT_STREQ(buf, "1234.123456");
	EXPECT_EQ(snprintf(buf, 128, "%Lf", 1234.123456L), 11);
	EXPECT_STREQ(buf, "1234.123456");
	EXPECT_EQ(snprintf(buf, 128, "%F", 1234.123456), 11);
	EXPECT_STREQ(buf, "1234.123456");
	EXPECT_EQ(snprintf(buf, 128, "%F", INFINITY), 3);
	EXPECT_STREQ(buf, "INF");
	EXPECT_EQ(snprintf(buf, 128, "%F", NAN), 3);
	EXPECT_STREQ(buf, "NAN");
	EXPECT_EQ(snprintf(buf, 128, "%lF", 1234.123456), 11);
	EXPECT_STREQ(buf, "1234.123456");
	EXPECT_EQ(snprintf(buf, 128, "%LF", 1234.123456L), 11);
	EXPECT_STREQ(buf, "1234.123456");

	EXPECT_EQ(snprintf(buf, 128, "%e", 1.123456), 12);
	EXPECT_STREQ(buf, "1.123456e+00");
	EXPECT_EQ(snprintf(buf, 128, "%e", 0.0), 12);
	EXPECT_STREQ(buf, "0.000000e+00");
	EXPECT_EQ(snprintf(buf, 128, "%e", -1.123456), 13);
	EXPECT_STREQ(buf, "-1.123456e+00");
	EXPECT_EQ(snprintf(buf, 128, "%e", 1.123456e12), 12);
	EXPECT_STREQ(buf, "1.123456e+12");
	EXPECT_EQ(snprintf(buf, 128, "%e", -1.123456e12), 13);
	EXPECT_STREQ(buf, "-1.123456e+12");
	EXPECT_EQ(snprintf(buf, 128, "%e", -1.123456e-12), 13);
	EXPECT_STREQ(buf, "-1.123456e-12");
	EXPECT_EQ(snprintf(buf, 128, "%e", INFINITY), 3);
	EXPECT_STREQ(buf, "inf");
	EXPECT_EQ(snprintf(buf, 128, "%e", NAN), 3);
	EXPECT_STREQ(buf, "nan");
	EXPECT_EQ(snprintf(buf, 128, "%le", 1.123456e12), 12);
	EXPECT_STREQ(buf, "1.123456e+12");
	EXPECT_EQ(snprintf(buf, 128, "%Le", 1.123456e12L), 12);
	EXPECT_STREQ(buf, "1.123456e+12");
	EXPECT_EQ(snprintf(buf, 128, "%E", 1.123456e12), 12);
	EXPECT_STREQ(buf, "1.123456E+12");
	EXPECT_EQ(snprintf(buf, 128, "%E", INFINITY), 3);
	EXPECT_STREQ(buf, "INF");
	EXPECT_EQ(snprintf(buf, 128, "%E", NAN), 3);
	EXPECT_STREQ(buf, "NAN");
	EXPECT_EQ(snprintf(buf, 128, "%lE", 1.123456e12), 12);
	EXPECT_STREQ(buf, "1.123456E+12");
	EXPECT_EQ(snprintf(buf, 128, "%LE", 1.123456e12L), 12);
	EXPECT_STREQ(buf, "1.123456E+12");

	EXPECT_EQ(snprintf(buf, 128, "%a", 0x1.123456p0), 13);
	EXPECT_STREQ(buf, "0x1.123456p+0");
	EXPECT_EQ(snprintf(buf, 128, "%a", 0.0), 6);
	EXPECT_STREQ(buf, "0x0p+0");
	EXPECT_EQ(snprintf(buf, 128, "%a", 0x1.123456p12), 14);
	EXPECT_STREQ(buf, "0x1.123456p+12");
	EXPECT_EQ(snprintf(buf, 128, "%a", -0x1.123456p-12), 15);
	EXPECT_STREQ(buf, "-0x1.123456p-12");
	EXPECT_EQ(snprintf(buf, 128, "%a", INFINITY), 3);
	EXPECT_STREQ(buf, "inf");
	EXPECT_EQ(snprintf(buf, 128, "%a", NAN), 3);
	EXPECT_STREQ(buf, "nan");
	EXPECT_EQ(snprintf(buf, 128, "%la", 0x1.123456p12), 14);
	EXPECT_STREQ(buf, "0x1.123456p+12");
	EXPECT_EQ(snprintf(buf, 128, "%La", 0x1.123456p12L), 12);
	EXPECT_STREQ(buf, "0x8.91a2bp+9");
	EXPECT_EQ(snprintf(buf, 128, "%A", 0x1.123456p12), 14);
	EXPECT_STREQ(buf, "0X1.123456P+12");
	EXPECT_EQ(snprintf(buf, 128, "%A", INFINITY), 3);
	EXPECT_STREQ(buf, "INF");
	EXPECT_EQ(snprintf(buf, 128, "%A", NAN), 3);
	EXPECT_STREQ(buf, "NAN");
	EXPECT_EQ(snprintf(buf, 128, "%lA", 0x1.123456p12), 14);
	EXPECT_STREQ(buf, "0X1.123456P+12");
	EXPECT_EQ(snprintf(buf, 128, "%LA", 0x1.123456p12L), 12);
	EXPECT_STREQ(buf, "0X8.91A2BP+9");

	EXPECT_EQ(snprintf(buf, 128, "%g", 1.1234), 6);
	EXPECT_STREQ(buf, "1.1234");
	EXPECT_EQ(snprintf(buf, 128, "%g", 0.0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%g", -1.1234), 7);
	EXPECT_STREQ(buf, "-1.1234");
	EXPECT_EQ(snprintf(buf, 128, "%g", 1.1234e12), 10);
	EXPECT_STREQ(buf, "1.1234e+12");
	EXPECT_EQ(snprintf(buf, 128, "%g", -1.1234e-12), 11);
	EXPECT_STREQ(buf, "-1.1234e-12");
	EXPECT_EQ(snprintf(buf, 128, "%g", INFINITY), 3);
	EXPECT_STREQ(buf, "inf");
	EXPECT_EQ(snprintf(buf, 128, "%g", NAN), 3);
	EXPECT_STREQ(buf, "nan");
	EXPECT_EQ(snprintf(buf, 128, "%lg", 1.1234e12), 10);
	EXPECT_STREQ(buf, "1.1234e+12");
	EXPECT_EQ(snprintf(buf, 128, "%Lg", 1.1234e12L), 10);
	EXPECT_STREQ(buf, "1.1234e+12");
	EXPECT_EQ(snprintf(buf, 128, "%G", 1.1234), 6);
	EXPECT_STREQ(buf, "1.1234");
	EXPECT_EQ(snprintf(buf, 128, "%G", 1.1234e12), 10);
	EXPECT_STREQ(buf, "1.1234E+12");
	EXPECT_EQ(snprintf(buf, 128, "%G", INFINITY), 3);
	EXPECT_STREQ(buf, "INF");
	EXPECT_EQ(snprintf(buf, 128, "%G", NAN), 3);
	EXPECT_STREQ(buf, "NAN");
	EXPECT_EQ(snprintf(buf, 128, "%lG", 1.1234e12), 10);
	EXPECT_STREQ(buf, "1.1234E+12");
	EXPECT_EQ(snprintf(buf, 128, "%LG", 1.1234e12L), 10);
	EXPECT_STREQ(buf, "1.1234E+12");

	signed char char_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%hhnc", &char_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(char_value, 2);
	short short_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%hnc", &short_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(short_value, 2);
	int int_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%nc", &int_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(int_value, 2);
	long long_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%lnc", &long_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(long_value, 2);
	long long long_long_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%llnc", &long_long_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(long_long_value, 2);
	intmax_t intmax_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%jnc", &intmax_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(intmax_value, 2);
	ssize_t size_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%jnc", &size_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(size_value, 2);
	ptrdiff_t ptrdiff_value = 0;
	EXPECT_EQ(snprintf(buf, 128, "ab%tnc", &ptrdiff_value), 3);
	EXPECT_STREQ(buf, "abc");
	EXPECT_EQ(ptrdiff_value, 2);

#if UINTPTR_MAX == UINT64_MAX
	uintptr_t ptr_value = 0xFFFFFFFFFFFFFFFF;
	EXPECT_EQ(snprintf(buf, 128, "%p", reinterpret_cast<void*>(ptr_value)), 18);
	EXPECT_STREQ(buf, "0xffffffffffffffff");
#else
	uintptr_t ptr_value = 0xFFFFFFFF;
	EXPECT_EQ(snprintf(buf, 128, "%p", reinterpret_cast<void*>(ptr_value)), 10);
	EXPECT_STREQ(buf, "0xffffffff");
#endif

	ptr_value = 0xFFFF;
	EXPECT_EQ(snprintf(buf, 128, "%p", reinterpret_cast<void*>(ptr_value)), 6);
	EXPECT_STREQ(buf, "0xffff");

	EXPECT_EQ(snprintf(buf, 128, "%-4dhello", 12), 9);
	EXPECT_STREQ(buf, "12  hello");

	EXPECT_EQ(snprintf(buf, 128, "%-4dhello", -12), 9);
	EXPECT_STREQ(buf, "-12 hello");

	EXPECT_EQ(snprintf(buf, 128, "%#-4xhello", 0x12), 9);
	EXPECT_STREQ(buf, "0x12hello");
	EXPECT_EQ(snprintf(buf, 128, "%#-4Xhello", 0x12), 9);
	EXPECT_STREQ(buf, "0X12hello");

	EXPECT_EQ(snprintf(buf, 128, "%#b", 0b1011), 6);
	EXPECT_STREQ(buf, "0b1011");
	EXPECT_EQ(snprintf(buf, 128, "%#B", 0b1011), 6);
	EXPECT_STREQ(buf, "0B1011");

	EXPECT_EQ(snprintf(buf, 128, "%+d", 1234), 5);
	EXPECT_STREQ(buf, "+1234");

	EXPECT_EQ(snprintf(buf, 128, "%+ d", 1234), 5);
	EXPECT_STREQ(buf, "+1234");

	EXPECT_EQ(snprintf(buf, 128, "% d", 1234), 5);
	EXPECT_STREQ(buf, " 1234");

	EXPECT_EQ(snprintf(buf, 128, "%4dhello", 12), 9);
	EXPECT_STREQ(buf, "  12hello");
	EXPECT_EQ(snprintf(buf, 128, "%2dhello", 1234), 9);
	EXPECT_STREQ(buf, "1234hello");
	int width = 4;
	EXPECT_EQ(snprintf(buf, 128, "%*dhello", width, 12), 9);
	EXPECT_STREQ(buf, "  12hello");
	width = -4;
	EXPECT_EQ(snprintf(buf, 128, "%*dhello", width, 12), 9);
	EXPECT_STREQ(buf, "12  hello");

	EXPECT_EQ(snprintf(buf, 128, "%04dhello", 12), 9);
	EXPECT_STREQ(buf, "0012hello");
	EXPECT_EQ(snprintf(buf, 128, "%-04dhello", 12), 9);
	EXPECT_STREQ(buf, "12  hello");
	EXPECT_EQ(snprintf(buf, 128, "%04.2dhello", 1), 9);
	EXPECT_STREQ(buf, "  01hello");

	EXPECT_EQ(snprintf(buf, 128, "%.d", 0), 0);
	EXPECT_STREQ(buf, "");
	EXPECT_EQ(snprintf(buf, 128, "%.0d", 0), 0);
	EXPECT_STREQ(buf, "");
	EXPECT_EQ(snprintf(buf, 128, "%04.d", 0), 4);
	EXPECT_STREQ(buf, "    ");
	EXPECT_EQ(snprintf(buf, 128, "%#04.x", 0), 4);
	EXPECT_STREQ(buf, "    ");
	EXPECT_EQ(snprintf(buf, 128, "%+04.d", 0), 4);
	EXPECT_STREQ(buf, "   +");
	int precision = 0;
	EXPECT_EQ(snprintf(buf, 128, "%.*d", precision, 0), 0);
	EXPECT_STREQ(buf, "");
	precision = -1;
	EXPECT_EQ(snprintf(buf, 128, "%.*d", precision, 0), 1);
	EXPECT_STREQ(buf, "0");
	EXPECT_EQ(snprintf(buf, 128, "%.6d", 1234), 6);
	EXPECT_STREQ(buf, "001234");

	EXPECT_EQ(snprintf(buf, 128, "%.o", 0), 0);
	EXPECT_STREQ(buf, "");
	EXPECT_EQ(snprintf(buf, 128, "%.0o", 0), 0);
	EXPECT_STREQ(buf, "");
}

TEST(ansi, scanf) {
	EXPECT_EQ(sscanf("", ""), 0);
	EXPECT_EQ(sscanf("", "a"), EOF);
	EXPECT_EQ(sscanf(" \t\na", " a"), 0);
	EXPECT_EQ(sscanf("a%", "a%%"), 0);
	EXPECT_EQ(sscanf("ab", "abcd"), EOF);
	EXPECT_EQ(sscanf("abab", "abcd"), 0);

	char char_value[8];
	wchar_t wchar_value[8];

	EXPECT_EQ(sscanf("a", "%c", char_value), 1);
	EXPECT_EQ(char_value[0], 'a');
	EXPECT_EQ(sscanf("a", "%lc", wchar_value), 1);
	EXPECT_EQ(wchar_value[0], L'a');
	EXPECT_EQ(sscanf("hello", "%s", char_value), 1);
	EXPECT_STREQ(char_value, "hello");
	EXPECT_EQ(sscanf("", "%s", char_value), EOF);
	EXPECT_EQ(sscanf(" ", "%s", char_value), EOF);
	EXPECT_EQ(sscanf(" a", "%sb", char_value), 1);
	EXPECT_EQ(sscanf("hello", "%ls", wchar_value), 1);
	EXPECT_STREQ(wchar_value, L"hello");
	EXPECT_EQ(sscanf("abcd", "%[ac]bcd", char_value), 1);
	EXPECT_STREQ(char_value, "a");
	EXPECT_EQ(sscanf("abcd", "%l[ac]bcd", wchar_value), 1);
	EXPECT_STREQ(wchar_value, L"a");
	EXPECT_EQ(sscanf("aacd", "%[ac]bcd", char_value), 1);
	EXPECT_STREQ(char_value, "aac");
	EXPECT_EQ(sscanf("aacd", "%[ac]bcd", char_value), 1);
	EXPECT_STREQ(char_value, "aac");
	EXPECT_EQ(sscanf("]", "%[]]", char_value), 1);
	EXPECT_STREQ(char_value, "]");
	EXPECT_EQ(sscanf("]", "%[^]]", char_value), 0);
	EXPECT_EQ(sscanf("abcd", "%[^ac]bcd", char_value), 0);
	EXPECT_EQ(sscanf("aacd", "%[^ac]bcd", char_value), 0);
	EXPECT_EQ(sscanf("aacd", "%[^ac]bcd", char_value), 0);
	EXPECT_EQ(sscanf("ddd", "%[^ac]bcd", char_value), 1);
	EXPECT_STREQ(char_value, "ddd");
	EXPECT_EQ(sscanf("abcd", "%[0-9]", char_value), 0);
	EXPECT_EQ(sscanf("2a", "%[0-9]", char_value), 1);
	EXPECT_STREQ(char_value, "2");

	unsigned char uchar_value;
	signed char schar_value;
	unsigned short ushort_value;
	short short_value;
	unsigned int uint_value;
	int int_value;
	unsigned long ulong_value;
	long long_value;
	unsigned long long ulonglong_value;
	long long longlong_value;
	uintmax_t uintmax_value;
	intmax_t intmax_value;
	size_t size_value;
	ptrdiff_t ptrdiff_value;

	EXPECT_EQ(sscanf("1234abc", "%d", &int_value), 1);
	EXPECT_EQ(int_value, 1234);
	EXPECT_EQ(sscanf("123", "%hhd", &schar_value), 1);
	EXPECT_EQ(schar_value, 123);
	EXPECT_EQ(sscanf("1234", "%hd", &short_value), 1);
	EXPECT_EQ(short_value, 1234);
	EXPECT_EQ(sscanf("1234", "%ld", &long_value), 1);
	EXPECT_EQ(long_value, 1234);
	EXPECT_EQ(sscanf("1234", "%lld", &longlong_value), 1);
	EXPECT_EQ(longlong_value, 1234);
	EXPECT_EQ(sscanf("1234", "%jd", &intmax_value), 1);
	EXPECT_EQ(intmax_value, 1234);
	EXPECT_EQ(sscanf("1234", "%zd", &size_value), 1);
	EXPECT_EQ(size_value, 1234);
	EXPECT_EQ(sscanf("1234", "%td", &ptrdiff_value), 1);
	EXPECT_EQ(ptrdiff_value, 1234);
	EXPECT_EQ(sscanf("    1235abc", "%d", &int_value), 1);
	EXPECT_EQ(int_value, 1235);
	EXPECT_EQ(sscanf("+1234abc", "%d", &int_value), 1);
	EXPECT_EQ(int_value, 1234);
	EXPECT_EQ(sscanf("-1234abc", "%d", &int_value), 1);
	EXPECT_EQ(int_value, -1234);
	EXPECT_EQ(sscanf("-abc", "%d", &int_value), 0);
	EXPECT_EQ(sscanf("-", "%d", &int_value), 0);

	EXPECT_EQ(sscanf("1234abc", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 1234);
	EXPECT_EQ(sscanf("123", "%hhi", &schar_value), 1);
	EXPECT_EQ(schar_value, 123);
	EXPECT_EQ(sscanf("1234", "%hi", &short_value), 1);
	EXPECT_EQ(short_value, 1234);
	EXPECT_EQ(sscanf("1234", "%li", &long_value), 1);
	EXPECT_EQ(long_value, 1234);
	EXPECT_EQ(sscanf("1234", "%lli", &longlong_value), 1);
	EXPECT_EQ(longlong_value, 1234);
	EXPECT_EQ(sscanf("1234", "%ji", &intmax_value), 1);
	EXPECT_EQ(intmax_value, 1234);
	EXPECT_EQ(sscanf("1234", "%zi", &size_value), 1);
	EXPECT_EQ(size_value, 1234);
	EXPECT_EQ(sscanf("1234", "%ti", &ptrdiff_value), 1);
	EXPECT_EQ(ptrdiff_value, 1234);
	EXPECT_EQ(sscanf("    1235abc", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 1235);
	EXPECT_EQ(sscanf("+1234abc", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 1234);
	EXPECT_EQ(sscanf("-1234abc", "%i", &int_value), 1);
	EXPECT_EQ(int_value, -1234);
	EXPECT_EQ(sscanf("-abc", "%i", &int_value), 0);

	EXPECT_EQ(sscanf(" 0b1011", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0b1011);
	EXPECT_EQ(sscanf(" 0B1010", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0b1010);
	EXPECT_EQ(sscanf(" +0b1011", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0b1011);
	EXPECT_EQ(sscanf(" -0b1111", "%i", &int_value), 1);
	EXPECT_EQ(int_value, -0b1111);

	EXPECT_EQ(sscanf(" 01234", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 01234);
	EXPECT_EQ(sscanf(" +01234", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 01234);
	EXPECT_EQ(sscanf(" -01234", "%i", &int_value), 1);
	EXPECT_EQ(int_value, -01234);

	EXPECT_EQ(sscanf(" 0xab12", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0xAB12);
	EXPECT_EQ(sscanf(" 0xAB12", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0xAB12);
	EXPECT_EQ(sscanf(" 0Xab12", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0xAB12);
	EXPECT_EQ(sscanf(" +0xab12", "%i", &int_value), 1);
	EXPECT_EQ(int_value, 0xAB12);
	EXPECT_EQ(sscanf(" -0xab12", "%i", &int_value), 1);
	EXPECT_EQ(int_value, -0xAB12);

	EXPECT_EQ(sscanf("1234abc", "%u", &uint_value), 1);
	EXPECT_EQ(uint_value, 1234);
	EXPECT_EQ(sscanf("123", "%hhu", &uchar_value), 1);
	EXPECT_EQ(uchar_value, 123);
	EXPECT_EQ(sscanf("1234", "%hu", &ushort_value), 1);
	EXPECT_EQ(ushort_value, 1234);
	EXPECT_EQ(sscanf("1234", "%lu", &ulong_value), 1);
	EXPECT_EQ(ulong_value, 1234);
	EXPECT_EQ(sscanf("1234", "%llu", &ulonglong_value), 1);
	EXPECT_EQ(ulonglong_value, 1234);
	EXPECT_EQ(sscanf("1234", "%ju", &uintmax_value), 1);
	EXPECT_EQ(uintmax_value, 1234);
	EXPECT_EQ(sscanf("1234", "%zu", &size_value), 1);
	EXPECT_EQ(size_value, 1234);
	EXPECT_EQ(sscanf("1234", "%tu", &ptrdiff_value), 1);
	EXPECT_EQ(ptrdiff_value, 1234);
	EXPECT_EQ(sscanf("    1235abc", "%u", &int_value), 1);
	EXPECT_EQ(int_value, 1235);
	EXPECT_EQ(sscanf("+1234abc", "%u", &int_value), 1);
	EXPECT_EQ(int_value, 1234);
	EXPECT_EQ(sscanf("-1234abc", "%u", &int_value), 1);
	EXPECT_EQ(int_value, -1234);
	EXPECT_EQ(sscanf("-abc", "%d", &int_value), 0);

	EXPECT_EQ(sscanf(" 12345", "%o", &int_value), 1);
	EXPECT_EQ(int_value, 012345);
	EXPECT_EQ(sscanf("12", "%hho", &uchar_value), 1);
	EXPECT_EQ(uchar_value, 012);
	EXPECT_EQ(sscanf("1234", "%ho", &ushort_value), 1);
	EXPECT_EQ(ushort_value, 01234);
	EXPECT_EQ(sscanf("1234", "%lo", &ulong_value), 1);
	EXPECT_EQ(ulong_value, 01234);
	EXPECT_EQ(sscanf("1234", "%llo", &ulonglong_value), 1);
	EXPECT_EQ(ulonglong_value, 01234);
	EXPECT_EQ(sscanf("1234", "%jo", &uintmax_value), 1);
	EXPECT_EQ(uintmax_value, 01234);
	EXPECT_EQ(sscanf("1234", "%zo", &size_value), 1);
	EXPECT_EQ(size_value, 01234);
	EXPECT_EQ(sscanf("1234", "%to", &ptrdiff_value), 1);
	EXPECT_EQ(ptrdiff_value, 01234);
	EXPECT_EQ(sscanf(" 01234", "%o", &int_value), 1);
	EXPECT_EQ(int_value, 01234);
	EXPECT_EQ(sscanf(" +01234", "%o", &int_value), 1);
	EXPECT_EQ(int_value, 01234);
	EXPECT_EQ(sscanf(" -01234", "%o", &int_value), 1);
	EXPECT_EQ(int_value, -01234);

	EXPECT_EQ(sscanf(" 1234ab", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234AB);
	EXPECT_EQ(sscanf("12", "%hhx", &uchar_value), 1);
	EXPECT_EQ(uchar_value, 0x12);
	EXPECT_EQ(sscanf("1234", "%hx", &ushort_value), 1);
	EXPECT_EQ(ushort_value, 0x1234);
	EXPECT_EQ(sscanf("1234", "%lx", &ulong_value), 1);
	EXPECT_EQ(ulong_value, 0x1234);
	EXPECT_EQ(sscanf("1234", "%llx", &ulonglong_value), 1);
	EXPECT_EQ(ulonglong_value, 0x1234);
	EXPECT_EQ(sscanf("1234", "%jx", &uintmax_value), 1);
	EXPECT_EQ(uintmax_value, 0x1234);
	EXPECT_EQ(sscanf("1234", "%zx", &size_value), 1);
	EXPECT_EQ(size_value, 0x1234);
	EXPECT_EQ(sscanf("1234", "%tx", &ptrdiff_value), 1);
	EXPECT_EQ(ptrdiff_value, 0x1234);
	EXPECT_EQ(sscanf(" 1234ABC", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234ABC);
	EXPECT_EQ(sscanf(" 1234ab", "%X", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234AB);
	EXPECT_EQ(sscanf(" 0x1234", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234);
	EXPECT_EQ(sscanf(" 0x1234A", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234A);
	EXPECT_EQ(sscanf(" 0X1234AB", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234AB);
	EXPECT_EQ(sscanf(" +0x1234", "%x", &int_value), 1);
	EXPECT_EQ(int_value, 0x1234);
	EXPECT_EQ(sscanf(" -0x1234", "%x", &int_value), 1);
	EXPECT_EQ(int_value, -0x1234);

	EXPECT_EQ(sscanf("abc", "abc%n", &int_value), 0);
	EXPECT_EQ(int_value, 3);
	EXPECT_EQ(sscanf("1234", "1234%hhn", &uchar_value), 0);
	EXPECT_EQ(uchar_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%hn", &ushort_value), 0);
	EXPECT_EQ(ushort_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%ln", &ulong_value), 0);
	EXPECT_EQ(ulong_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%lln", &ulonglong_value), 0);
	EXPECT_EQ(ulonglong_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%jn", &uintmax_value), 0);
	EXPECT_EQ(uintmax_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%zn", &size_value), 0);
	EXPECT_EQ(size_value, 4);
	EXPECT_EQ(sscanf("1234", "1234%tn", &ptrdiff_value), 0);
	EXPECT_EQ(ptrdiff_value, 4);
	EXPECT_EQ(sscanf("abcd", "ab%nc%c", &int_value, char_value), 1);
	EXPECT_EQ(int_value, 2);
	EXPECT_EQ(char_value[0], 'd');

	float float_value;
	double double_value;
	long double long_double_value;
	EXPECT_EQ(sscanf("1.123", "%a", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.123f);
	EXPECT_EQ(sscanf("1.123", "%la", &double_value), 1);
	EXPECT_FLOAT_EQ(double_value, 1.123);
	EXPECT_EQ(sscanf("1.123", "%La", &long_double_value), 1);
	EXPECT_FLOAT_EQ(long_double_value, 1.123L);
	EXPECT_EQ(sscanf("1.1234", "%A", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234f);
	EXPECT_EQ(sscanf("1.123", "%e", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.123f);
	EXPECT_EQ(sscanf("1.123", "%le", &double_value), 1);
	EXPECT_FLOAT_EQ(double_value, 1.123);
	EXPECT_EQ(sscanf("1.123", "%Le", &long_double_value), 1);
	EXPECT_FLOAT_EQ(long_double_value, 1.123L);
	EXPECT_EQ(sscanf("1.1234", "%E", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234f);
	EXPECT_EQ(sscanf("1.123", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.123f);
	EXPECT_EQ(sscanf("1.123", "%lf", &double_value), 1);
	EXPECT_FLOAT_EQ(double_value, 1.123);
	EXPECT_EQ(sscanf("1.123", "%Lf", &long_double_value), 1);
	EXPECT_FLOAT_EQ(long_double_value, 1.123L);
	EXPECT_EQ(sscanf("1.1234", "%F", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234f);
	EXPECT_EQ(sscanf("1.123", "%g", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.123f);
	EXPECT_EQ(sscanf("1.123", "%lg", &double_value), 1);
	EXPECT_FLOAT_EQ(double_value, 1.123);
	EXPECT_EQ(sscanf("1.123", "%Lg", &long_double_value), 1);
	EXPECT_FLOAT_EQ(long_double_value, 1.123L);
	EXPECT_EQ(sscanf("1.1234", "%G", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234f);
	EXPECT_EQ(sscanf("+1.1234", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234f);
	EXPECT_EQ(sscanf("-1.1234", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, -1.1234f);
	EXPECT_EQ(sscanf("1.1234e3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234E3f);
	EXPECT_EQ(sscanf("1.1234E3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.1234E3f);
	EXPECT_EQ(sscanf("0x1234", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1234P0f);
	EXPECT_EQ(sscanf("0X1234", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1234P0f);
	EXPECT_EQ(sscanf("0x1.1234", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1.1234P0);
	EXPECT_EQ(sscanf("0x1.abcdP3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1.ABCDP3);
	EXPECT_EQ(sscanf("0x1.abcdP+3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1.ABCDP3);
	EXPECT_EQ(sscanf("0x1.abcdP-3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1.ABCDP-3);
	EXPECT_EQ(sscanf("0x1.AbCdP-3", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1.ABCDP-3);
	EXPECT_EQ(sscanf("inf", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("InF", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("+inf", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("-inf", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("INF", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("infinity", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("INFINITY", "%f", &float_value), 1);
	EXPECT_TRUE(isinf(float_value));
	EXPECT_EQ(sscanf("nan", "%f", &float_value), 1);
	EXPECT_TRUE(isnan(float_value));
	EXPECT_EQ(sscanf("NaN", "%f", &float_value), 1);
	EXPECT_TRUE(isnan(float_value));
	EXPECT_EQ(sscanf("+nan", "%f", &float_value), 1);
	EXPECT_TRUE(isnan(float_value));
	EXPECT_EQ(sscanf("-nan", "%f", &float_value), 1);
	EXPECT_TRUE(isnan(float_value));
	EXPECT_EQ(sscanf("0xhello", "%f", &float_value), 0);
	EXPECT_EQ(sscanf("0x1hello", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 0x1P0f);
	EXPECT_EQ(sscanf("ab1.1234", "%f", &float_value), 0);
	EXPECT_EQ(sscanf("  1.123", "%f", &float_value), 1);
	EXPECT_FLOAT_EQ(float_value, 1.123f);

	void* ptr;
	EXPECT_EQ(sscanf("hello", "%p", &ptr), 0);
#if UINTPTR_MAX == UINT64_MAX
	EXPECT_EQ(sscanf("0xabcdef12abcdef12", "%p", &ptr), 1);
	EXPECT_EQ(ptr, reinterpret_cast<void*>(0xabcdef12abcdef12));
#else
	EXPECT_EQ(sscanf("0xabcdef12", "%p", &ptr), 1);
	EXPECT_EQ(ptr, reinterpret_cast<void*>(0xabcdef12));
#endif

	EXPECT_EQ(sscanf("a", "%*c"), 0);
	char_value[2] = 'q';
	EXPECT_EQ(sscanf("abcd", "%2c", char_value), 1);
	EXPECT_EQ(char_value[2], 'q');
	char_value[2] = 0;
	EXPECT_STREQ(char_value, "ab");

	EXPECT_EQ(sscanf("cdef", "%2s", char_value), 1);
	EXPECT_STREQ(char_value, "cd");
	EXPECT_EQ(sscanf("c def", "%2s", char_value), 1);
	EXPECT_STREQ(char_value, "c");

	EXPECT_EQ(sscanf("aacd", "%2[ac]", char_value), 1);
	EXPECT_STREQ(char_value, "aa");
}

TEST(ansi, strtof) {
	char* end;
	float value = strtof("5.008", &end);
	EXPECT_FLOAT_EQ(value, 5.008);
	double double_value = strtod("5.008", &end);
	EXPECT_DOUBLE_EQ(double_value, 5.008);
	long double long_double_value = strtold("5.008", &end);
	EXPECT_FLOAT_EQ(long_double_value, 5.008);
	value = strtof("5.006", &end);
	EXPECT_FLOAT_EQ(value, 5.006);
	value = strtof("1234.123456", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.123456);
	value = strtof("         1234.123456", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.123456);
	value = strtof("+1234.1234", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.1234);
	value = strtof("-1234.1234", nullptr);
	EXPECT_FLOAT_EQ(value, -1234.1234);
	value = strtof("1234.123E3", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.123E3);
	value = strtof("1234.123E+3", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.123E3);
	value = strtof("1234.123E-3", nullptr);
	EXPECT_FLOAT_EQ(value, 1234.123E-3);

	value = strtof("0x1234", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234);
	value = strtof("+0x1234", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234);
	value = strtof("-0x1234", nullptr);
	EXPECT_FLOAT_EQ(value, -0x1234);
	value = strtof("0x1234.123P0", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P0);
	value = strtof("0x1234.123p0", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P0);
	value = strtof("0x1234.123P3", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P3);
	value = strtof("0x1234.123P+3", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P3);
	value = strtof("0x1234.123P-3", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P-3);
	value = strtof("-0x1234.123P-3", nullptr);
	EXPECT_FLOAT_EQ(value, -0x1234.123P-3);
	value = strtof("0x1234.123P10", nullptr);
	EXPECT_FLOAT_EQ(value, 0x1234.123P10);
	value = strtof("InF", nullptr);
	EXPECT_EQ(isinf(value), true);
	value = strtof("InFiNiTy", nullptr);
	EXPECT_EQ(isinf(value), true);
	value = strtof("NaN", nullptr);
	EXPECT_EQ(isnan(value), true);
}
