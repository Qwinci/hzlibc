#include "time.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "sys.hpp"
#include "fcntl.h"
#include "limits.h"
#include "errno.h"
#include "internal/time.hpp"
#include <hz/vector.hpp>
#include <hz/bit.hpp>
#include <hz/string_view.hpp>

namespace {
	constexpr bool LOG_MISSING_LOCALTIME = false;
}

#define memcpy __builtin_memcpy
#define strlen __builtin_strlen

struct TzFile {
	char magic[4];
	uint8_t version;
	char reserved[15];
	uint32_t tzh_ttisutcnt;
	uint32_t tzh_ttisstdcnt;
	uint32_t tzh_leapcnt;
	uint32_t tzh_timecnt;
	uint32_t tzh_typecnt;
	uint32_t tzh_charcnt;
};

struct [[gnu::packed]] TtInfo {
	int32_t tt_utoff;
	unsigned char tt_isdst;
	unsigned char tt_desigidx;
};

namespace {
	[[gnu::init_priority(101)]] hz::vector<uint8_t, Allocator> TZ_DATA {Allocator {}};

	[[gnu::constructor(102)]] void init_timezone() {
		int fd;
		if (sys_openat(AT_FDCWD, "/etc/localtime", O_RDONLY, 0, &fd)) {
			if constexpr (LOG_MISSING_LOCALTIME) {
				println("init_timezone: failed to open /etc/localtime");
			}
			return;
		}

		struct stat64 s {};
		if (sys_stat(StatTarget::Fd, fd, "", 0, &s)) {
			println("init_timezone: failed to stat /etc/localtime");
			__ensure(sys_close(fd) == 0);
			return;
		}

		TZ_DATA.resize(s.st_size);
		ssize_t count_read;
		if (sys_read(fd, TZ_DATA.data(), static_cast<size_t>(s.st_size), &count_read)) {
			println("init_timezone: failed to read /etc/localtime");
			__ensure(sys_close(fd) == 0);
			TZ_DATA.clear();
			TZ_DATA.shrink_to_fit();
			return;
		}

		__ensure(sys_close(fd) == 0);
		__ensure(static_cast<off64_t>(count_read) == s.st_size);

		auto* ptr = reinterpret_cast<TzFile*>(TZ_DATA.data());
		__ensure(ptr->magic[0] == 'T' && ptr->magic[1] == 'Z' &&
			ptr->magic[2] == 'i' && ptr->magic[3] == 'f');
		__ensure(ptr->version == 0 || ptr->version == '2' ||
			ptr->version == '3' || ptr->version == '4');
		ptr->tzh_ttisutcnt = hz::to_ne_from_be(ptr->tzh_ttisutcnt);
		ptr->tzh_ttisstdcnt = hz::to_ne_from_be(ptr->tzh_ttisstdcnt);
		ptr->tzh_leapcnt = hz::to_ne_from_be(ptr->tzh_leapcnt);
		ptr->tzh_timecnt = hz::to_ne_from_be(ptr->tzh_timecnt);
		ptr->tzh_typecnt = hz::to_ne_from_be(ptr->tzh_typecnt);
		ptr->tzh_charcnt = hz::to_ne_from_be(ptr->tzh_charcnt);
	}
}

bool tz_data_initialized() {
	return !TZ_DATA.empty();
}

TzData get_tz_data(time_t time) {
	auto* ptr = reinterpret_cast<TzFile*>(TZ_DATA.data());

	int transition_index = -1;
	for (uint32_t i = 0; i < ptr->tzh_timecnt; ++i) {
		int32_t transition_time = hz::to_ne_from_be(
			reinterpret_cast<int32_t*>(&ptr[1])[i]);
		if (i && transition_time > time) {
			transition_index = static_cast<int>(i - 1);
			break;
		}
	}

	uint8_t transition_info_index = 0;
	if (transition_index >= 0) {
		transition_info_index = reinterpret_cast<uint8_t*>(&ptr[1])[
			ptr->tzh_timecnt * 4 + transition_index];
	}

	__ensure(ptr->tzh_typecnt);

	TtInfo info {};
	memcpy(&info,
	       reinterpret_cast<char*>(&ptr[1]) +
	       ptr->tzh_timecnt * 5 +
	       transition_info_index * sizeof(TtInfo),
	       sizeof(TtInfo));
	info.tt_utoff = hz::to_ne_from_be(info.tt_utoff);

	auto* abbrevs = reinterpret_cast<char*>(&ptr[1]) +
	                ptr->tzh_timecnt * 5 +
	                ptr->tzh_typecnt * sizeof(TtInfo);
	return {
		.gmt_offset = info.tt_utoff,
		.is_dst = static_cast<bool>(info.tt_isdst),
		.zone = abbrevs + info.tt_desigidx
	};
}

EXPORT time_t time(time_t* arg) {
	timespec tp {};
	if (auto err = sys_clock_gettime(CLOCK_REALTIME, &tp)) {
		errno = err;
		return -1;
	}
	time_t value = tp.tv_sec + (tp.tv_nsec / (1000 * 1000 * 1000));

	if (arg) {
		*arg = value;
	}
	return value;
}

EXPORT clock_t clock() {
	timespec tp {};
	if (auto err = sys_clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp)) {
		errno = err;
		return -1;
	}
	return tp.tv_sec * CLOCKS_PER_SEC + tp.tv_nsec / 1000;
}

EXPORT struct tm* localtime(const time_t* time) {
	static tm t {};
	return localtime_r(time, &t);
}

EXPORT tm* localtime_r(const time_t* __restrict time, tm* __restrict buf) {
	auto time_value = *time;

#if UINTPTR_MAX == UINT64_MAX
	// 3000
	if (time_value < 0 || time_value >= 32500915200) {
		errno = EOVERFLOW;
		return nullptr;
	}
#endif

	int32_t gmt_offset = 0;
	bool is_dst = false;
	const char* zone = "UTC";
	if (!TZ_DATA.empty()) {
		auto info = get_tz_data(time_value);
		gmt_offset = info.gmt_offset;
		is_dst = info.is_dst;
		zone = info.zone;
	}

	// Thu 1.1.1970 00.00
	time_t local = time_value + gmt_offset;
	time_t days_since_epoch = local / (60 * 60 * 24);

	int days_since_sunday = static_cast<int>(
		days_since_epoch >= -4 ?
		(days_since_epoch + 4) % 7
		: ((days_since_epoch + 5) % 7 + 6));

	int year = EPOCH_YEAR;
	while (true) {
		auto days = days_per_year(year);
		if (days_since_epoch >= days) {
			days_since_epoch -= days;
			++year;
		}
		else {
			break;
		}
	}

	int year_day = static_cast<int>(days_since_epoch);

	auto days_per_month_table = get_days_per_month_table(year);
	int month = 0;
	while (true) {
		auto days = days_per_month_table[month];
		if (days_since_epoch >= days) {
			days_since_epoch -= days;
			++month;
		}
		else {
			break;
		}
	}

	int day = static_cast<int>(days_since_epoch) + 1;

	int seconds = static_cast<int>(local % 60);
	int minutes = static_cast<int>(local / 60 % 60);
	int hours = static_cast<int>(local / 60 / 60 % 24);

	buf->tm_sec = seconds;
	buf->tm_min = minutes;
	buf->tm_hour = hours;
	buf->tm_mday = day;
	buf->tm_mon = month;
	buf->tm_year = year - 1900;
	buf->tm_wday = days_since_sunday;
	buf->tm_yday = year_day;
	buf->tm_isdst = is_dst;
	buf->tm_gmtoff = gmt_offset;
	buf->tm_zone = zone;
	return buf;
}

namespace {
	struct tm GMTIME_BUF {};
}

EXPORT struct tm* gmtime(const time_t* __restrict time) {
	return gmtime_r(time, &GMTIME_BUF);
}

EXPORT struct tm* gmtime_r(const time_t* __restrict time, struct tm* __restrict buf) {
	time_t time_value = *time;
	time_t days_since_epoch = time_value / (60 * 60 * 24);

#if UINTPTR_MAX == UINT64_MAX
	// 3000
	if (time_value < -32500915200 || time_value >= 32500915200) {
		errno = EOVERFLOW;
		return nullptr;
	}
#endif

	int days_since_sunday = static_cast<int>(
		days_since_epoch >= -4 ?
		(days_since_epoch + 4) % 7
		: ((days_since_epoch + 5) % 7 + 6));

	int year = EPOCH_YEAR;
	while (true) {
		auto days = days_per_year(year);
		if (days_since_epoch >= days) {
			days_since_epoch -= days;
			++year;
		}
		else {

			break;
		}
	}

	int year_day = static_cast<int>(days_since_epoch);

	auto days_per_month_table = get_days_per_month_table(year);
	int month = 0;
	while (true) {
		auto days = days_per_month_table[month];
		if (days_since_epoch >= days) {
			days_since_epoch -= days;
			++month;
		}
		else {
			break;
		}
	}

	int day = static_cast<int>(days_since_epoch) + 1;

	int seconds = static_cast<int>(time_value % 60);
	int minutes = static_cast<int>(time_value / 60 % 60);
	int hours = static_cast<int>(time_value / 60 / 60 % 24);

	buf->tm_sec = seconds;
	buf->tm_min = minutes;
	buf->tm_hour = hours;
	buf->tm_mday = day;
	buf->tm_mon = month;
	buf->tm_year = year - 1900;
	buf->tm_wday = days_since_sunday;
	buf->tm_yday = year_day;
	buf->tm_isdst = 0;
	buf->tm_gmtoff = 0;
	buf->tm_zone = const_cast<char*>("UTC");
	return buf;
}

EXPORT time_t mktime(struct tm* arg) {
	int sec = arg->tm_sec;
	int min = arg->tm_min;
	int hour = arg->tm_hour;
	int day = arg->tm_mday - 1;
	int mon = arg->tm_mon;
	int year = arg->tm_year + 1900;

	while (sec >= 60) {
		sec -= 60;
		if (min == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++min;
	}
	while (min >= 60) {
		min -= 60;
		if (hour == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++hour;
	}
	while (hour >= 24) {
		hour -= 24;
		if (day == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++day;
	}
	while (mon >= 12) {
		mon -= 12;
		if (year == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++year;
	}

	while (true) {
		auto days_per_month_table = get_days_per_month_table(year);
		if (day < days_per_month_table[mon]) {
			break;
		}
		day -= days_per_month_table[mon];
		++mon;
		if (mon == 12) {
			mon = 0;
			++year;
		}
	}

	arg->tm_sec = sec;
	arg->tm_min = min;
	arg->tm_hour = hour;
	arg->tm_mday = day + 1;
	arg->tm_mon = mon;
	arg->tm_year = year - 1900;
	if (arg->tm_isdst > 0) {
		arg->tm_isdst = 1;
	}

	time_t days_since_epoch = arg->tm_mday - 1;

	auto days_per_month_table = get_days_per_month_table(arg->tm_year + 1900);
	for (int month = 0; month < arg->tm_mon; ++month) {
		days_since_epoch += days_per_month_table[month];
	}

	arg->tm_yday = static_cast<int>(days_since_epoch);

	for (year = EPOCH_YEAR; year < arg->tm_year + 1900; ++year) {
		days_since_epoch += days_per_year(year);
	}

	int days_since_sunday = static_cast<int>(
		days_since_epoch >= -4 ?
		(days_since_epoch + 4) % 7
		: ((days_since_epoch + 5) % 7 + 6));
	arg->tm_wday = days_since_sunday;

	time_t time = days_since_epoch * (60 * 60 * 24) + arg->tm_hour * (60 * 60) + arg->tm_min * 60 + arg->tm_sec;

	if (!TZ_DATA.empty()) {
		auto info = get_tz_data(time);
		if (arg->tm_isdst < 0) {
			arg->tm_isdst = info.is_dst;
		}
		arg->tm_gmtoff = info.gmt_offset;
		arg->tm_zone = info.zone;
		time -= info.gmt_offset;
	}
	else {
		if (arg->tm_isdst < 0) {
			arg->tm_isdst = -1;
		}
	}

	return time;
}

static constexpr char CHARS[] = "0123456789ABCDEF";
static constexpr hz::string_view MONTH_ABBREVS[] {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static constexpr hz::string_view MONTHS[] {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

static constexpr hz::string_view DAY_ABBREVS[] {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

static constexpr hz::string_view DAYS[] {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

static constexpr hz::string_view FIN_MONTH_ABBREVS[] {
	"tammi",
	"helmi",
	"maalis",
	"huhti",
	"touko",
	"kesä",
	"heinä",
	"elo",
	"syys",
	"loka",
	"marras",
	"joulu"
};

static constexpr hz::string_view FIN_MONTHS[] {
	"tammikuu",
	"helmikuu",
	"maaliskuu",
	"huhtikuu",
	"toukokuu",
	"kesäkuu",
	"heinäkuu",
	"elokuu",
	"syyskuu",
	"lokakuu",
	"marraskuu",
	"joulukuu"
};

EXPORT size_t strftime(char* __restrict str, size_t count, const char* __restrict fmt, const struct tm* time) {
	// todo this should properly take locale into account

	size_t written = 0;

	auto write = [&](const char* ptr, size_t len) {
		if (written + len > count) {
			return false;
		}
		memcpy(str, ptr, len);
		str += len;
		written += len;
		return true;
	};

	auto write_int = [&](int value, int base, int pad) {
		char buf[64];
		char* ptr = buf + 64;
		do {
			*--ptr = CHARS[value % base];
			value /= base;
		} while (value);
		int len = static_cast<int>((buf + 64) - ptr);
		if (len < pad) {
			pad -= len;
			for (; pad; --pad) {
				*--ptr = '0';
			}
		}

		return write(ptr, (buf + 64) - ptr);
	};

	while (true) {
		auto* start = fmt;
		for (; *fmt && *fmt != '%'; ++fmt);
		size_t len = fmt - start;
		for (; *fmt == '%' && fmt[1] == '%'; fmt += 2) ++len;
		if (len && !write(start, len)) {
			return 0;
		}

		if (!*fmt) {
			break;
		}

		++fmt;
		switch (*fmt) {
			case 'n':
				++fmt;
				if (!write("\n", 1)) {
					return 0;
				}
				break;
			case 't':
				++fmt;
				if (!write("\t", 1)) {
					return 0;
				}
				break;
			case 'Y':
				++fmt;
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case 'E':
				++fmt;
				if (*fmt == 'Y') {
					++fmt;
					if (!write_int(time->tm_year + 1900, 10, 0)) {
						return 0;
					}
				}
				else if (*fmt == 'y') {
					++fmt;
					// todo this should be the offset from nl_langinfo with ERA
					int year = time->tm_year + 1900;
					int era = year / 1000 * 1000;
					year -= era;
					if (!write_int(year, 10, 2)) {
						return 0;
					}
				}
				else if (*fmt == 'C') {
					++fmt;
				}
				else if (*fmt == 'c') {
					++fmt;
				}
				else if (*fmt == 'X') {
					++fmt;
				}
				else {
					return 0;
				}
				break;
			case 'y':
			{
				++fmt;
				int year = time->tm_year + 1900;
				char chars[] {CHARS[year / 10 % 10], CHARS[year % 10]};
				if (!write(chars, 2)) {
					return 0;
				}
				break;
			}
			case 'O':
				++fmt;
				if (*fmt == 'y') {
					++fmt;
				}
				else if (*fmt == 'b') {
					++fmt;
				}
				else if (*fmt == 'B') {
					++fmt;
				}
				else if (*fmt == 'm') {
					++fmt;
				}
				else if (*fmt == 'd') {
					++fmt;
				}
				else if (*fmt == 'e') {
					++fmt;
				}
				else if (*fmt == 'w') {
					++fmt;
				}
				else if (*fmt == 'u') {
					++fmt;
				}
				else if (*fmt == 'H') {
					++fmt;
				}
				else if (*fmt == 'I') {
					++fmt;
				}
				else if (*fmt == 'M') {
					++fmt;
				}
				else if (*fmt == 'S') {
					++fmt;
				}
				else {
					return 0;
				}
				break;
			case 'C':
			{
				++fmt;
				int year = time->tm_year + 1900;
				char chars[] {CHARS[year / 10 / 10 / 10 % 10], CHARS[year / 10 / 10 % 10]};
				if (!write(chars, 2)) {
					return 0;
				}
				break;
			}
			case 'G':
			case 'g':
			{
				++fmt;
				// todo
				break;
			}
			case 'b':
			case 'h':
				++fmt;
				if (!write(MONTH_ABBREVS[time->tm_mon].data(), MONTH_ABBREVS[time->tm_mon].size())) {
					return 0;
				}
				break;
			case 'B':
				++fmt;
				if (!write(MONTHS[time->tm_mon].data(), MONTHS[time->tm_mon].size())) {
					return 0;
				}
				break;
			case 'm':
				++fmt;
				if (!write_int(time->tm_mon + 1, 10, 2)) {
					return 0;
				}
				break;
			case 'j':
				++fmt;
				if (!write_int(time->tm_yday + 1, 10, 3)) {
					return 0;
				}
				break;
			case 'd':
				++fmt;
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				break;
			case 'e':
			{
				++fmt;
				char chars[] {CHARS[time->tm_mday / 10 % 10], CHARS[time->tm_mday % 10]};
				if (chars[0] == '0') {
					if (!write(&chars[1], 1)) {
						return 0;
					}
				}
				else {
					if (!write(chars, 2)) {
						return 0;
					}
				}
				break;
			}
			case 'a':
				++fmt;
				if (!write(DAY_ABBREVS[time->tm_wday].data(), DAY_ABBREVS[time->tm_wday].size())) {
					return 0;
				}
				break;
			case 'A':
				++fmt;
				if (!write(DAYS[time->tm_wday].data(), DAYS[time->tm_wday].size())) {
					return 0;
				}
				break;
			case 'w':
				++fmt;
				if (!write_int(time->tm_wday, 10, 0)) {
					return 0;
				}
				break;
			case 'u':
				++fmt;
				if (!write_int(time->tm_wday + 1, 10, 0)) {
					return 0;
				}
				break;
			case 'H':
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				break;
			case 'I':
			{
				++fmt;
				int hour = time->tm_hour % 12;
				if (hour == 0) {
					hour = 12;
				}
				if (!write_int(hour, 10, 0)) {
					return 0;
				}
				break;
			}
			case 'M':
				++fmt;
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			case 'S':
				++fmt;
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				break;
			case 'c':
			{
				++fmt;
				if (!write(DAY_ABBREVS[time->tm_wday].data(), DAY_ABBREVS[time->tm_wday].size())) {
					return 0;
				}
				if (!write(" ", 1)) {
					return 0;
				}
				if (!write(MONTH_ABBREVS[time->tm_mon].data(), MONTH_ABBREVS[time->tm_mon].size())) {
					return 0;
				}
				if (!write(" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 0)) {
					return 0;
				}
				if (!write(" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				if (!write(" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			}
			case 'x':
				++fmt;
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				if (!write(".", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write(".", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case 'D':
				++fmt;
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write("/", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				if (!write("/", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case 'F':
				++fmt;
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				if (!write("-", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write("-", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				break;
			case 'r':
			{
				++fmt;
				int hour = time->tm_hour % 12;
				if (hour == 0) {
					hour = 12;
				}
				if (!write_int(hour, 10, 0)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			}
			case 'R':
			{
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			}
			case 'T':
			{
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				if (!write(":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				break;
			}
			case 'p':
				if (time->tm_hour < 12) {
					if (!write("AM", 2)) {
						return 0;
					}
				}
				else {
					if (!write("PM", 2)) {
						return 0;
					}
				}
				break;
			case 'z':
			{
				if (!TZ_DATA.empty()) {
					tm copy = *time;
					auto t = mktime(&copy);
					if (t != -1) {
						auto info = get_tz_data(t);
						if (info.gmt_offset < 0) {
							info.gmt_offset *= -1;
							if (!write("-", 1)) {
								return 0;
							}
						}
						else {
							if (!write("+", 1)) {
								return 0;
							}
						}
						int hour = info.gmt_offset / (60 * 60);
						int min = info.gmt_offset % (60 * 60) / 60;

						if (!write_int(hour, 10, 2)) {
							return 0;
						}
						if (!write_int(min, 10, 2)) {
							return 0;
						}
					}
				}
				break;
			}
			case 'Z':
			{
				if (!TZ_DATA.empty()) {
					tm copy = *time;
					auto t = mktime(&copy);
					if (t != -1) {
						auto info = get_tz_data(t);
						if (!write(info.zone, strlen(info.zone))) {
							return 0;
						}
					}
				}
				break;
			}
			default:
				if (!write(fmt - 1, 2)) {
					return 0;
				}
				++fmt;
				break;
		}
	}

	if (count) {
		if (written < count) {
			*str = 0;
		}
		else {
			return 0;
		}
	}

	return written;
}

namespace {
	char ASCTIME_BUF[26] {};
}

EXPORT char* asctime(const tm* time) {
	strftime(ASCTIME_BUF, 26, "%a %b %d %T %Y\n", time);
	return ASCTIME_BUF;
}

EXPORT char* ctime(const time_t* time) {
	return asctime(localtime(time));
}

EXPORT double difftime(time_t b, time_t a) {
	if (a > 0) {
		if (b < LONG_MIN - a) {
			return static_cast<double>(LONG_MIN);
		}
	}
	else {
		if (b > LONG_MAX - a) {
			return static_cast<double>(LONG_MAX);
		}
	}
	return static_cast<double>(b) - static_cast<double>(a);
}

EXPORT int timespec_get(struct timespec* ts, int base) {
	if (base != TIME_UTC) {
		return 0;
	}
	int res = clock_gettime(CLOCK_REALTIME, ts);
	return res < 0 ? 0 : base;
}

// posix

EXPORT time_t timegm(struct tm* arg) {
	int sec = arg->tm_sec;
	int min = arg->tm_min;
	int hour = arg->tm_hour;
	int day = arg->tm_mday - 1;
	int mon = arg->tm_mon;
	int year = arg->tm_year + 1900;

	while (sec >= 60) {
		sec -= 60;
		if (min == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++min;
	}
	while (min >= 60) {
		min -= 60;
		if (hour == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++hour;
	}
	while (hour >= 24) {
		hour -= 24;
		if (day == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++day;
	}
	while (mon >= 12) {
		mon -= 12;
		if (year == INT_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		++year;
	}

	while (true) {
		auto days_per_month_table = get_days_per_month_table(year);
		if (day < days_per_month_table[mon]) {
			break;
		}
		day -= days_per_month_table[mon];
		++mon;
		if (mon == 12) {
			mon = 0;
			++year;
		}
	}

	arg->tm_sec = sec;
	arg->tm_min = min;
	arg->tm_hour = hour;
	arg->tm_mday = day + 1;
	arg->tm_mon = mon;
	arg->tm_year = year - 1900;

	time_t days_since_epoch = arg->tm_mday - 1;

	auto days_per_month_table = get_days_per_month_table(arg->tm_year + 1900);
	for (int month = 0; month < arg->tm_mon; ++month) {
		days_since_epoch += days_per_month_table[month];
	}

	arg->tm_yday = static_cast<int>(days_since_epoch);

	for (year = EPOCH_YEAR; year < arg->tm_year + 1900; ++year) {
		days_since_epoch += days_per_year(year);
	}

	int days_since_sunday = static_cast<int>(
		days_since_epoch >= -4 ?
		(days_since_epoch + 4) % 7
		                       : ((days_since_epoch + 5) % 7 + 6));
	arg->tm_wday = days_since_sunday;

	time_t time = days_since_epoch * (60 * 60 * 24) + arg->tm_hour * (60 * 60) + arg->tm_min * 60 + arg->tm_sec;

	arg->tm_isdst = 0;
	arg->tm_gmtoff = 0;
	arg->tm_zone = const_cast<char*>("UTC");

	return time;
}
