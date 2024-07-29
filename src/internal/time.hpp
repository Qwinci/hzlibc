#pragma once
#include "time.h"

namespace {
	constexpr bool is_leap(time_t year) {
		return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
	}

	constexpr int days_per_year(time_t year) {
		return is_leap(year) ? 366 : 365;
	}

	constexpr int DAYS_IN_MONTHS_NOLEAP[] {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	constexpr int DAYS_IN_MONTHS_LEAP[] {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	constexpr const int* get_days_per_month_table(int year) {
		return is_leap(year) ? DAYS_IN_MONTHS_LEAP : DAYS_IN_MONTHS_NOLEAP;
	}

	constexpr int EPOCH_YEAR = 1970;
}

struct TzData {
	int32_t gmt_offset;
	bool is_dst;
	const char* zone;
};

bool tz_data_initialized();
TzData get_tz_data(time_t time);
