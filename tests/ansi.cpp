#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>

TEST(ansi, printf) {
	char buf[128];
	snprintf(buf, 128, "invalid %%-code\n");
	EXPECT_STREQ(buf, "invalid %-code\n");
}

TEST(ansi, scanf) {
	float value;
	sscanf("1.1234", "%f", &value);
	EXPECT_FLOAT_EQ(value, 1.1234f);
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

	char buf[32];
	snprintf(buf, 32, "%.6f", 123.123456);
	EXPECT_STREQ(buf, "123.123456");
	snprintf(buf, 32, "%.1f", 5.12);
	EXPECT_STREQ(buf, "5.1");
	snprintf(buf, 32, "%.0f", 5.00);
	EXPECT_STREQ(buf, "5");
	snprintf(buf, 32, "%.1f", 5.00);
	EXPECT_STREQ(buf, "5.0");

	EXPECT_EQ(strtol("5.008", &end, 10), 5);
	EXPECT_EQ(strtol(end + 1, &end, 10), 8);

	int int_value0;
	int int_value1;
	EXPECT_EQ(sscanf("5.008", "%d.%d", &int_value0, &int_value1), 2);
	EXPECT_EQ(int_value0, 5);
	EXPECT_EQ(int_value1, 8);
}
