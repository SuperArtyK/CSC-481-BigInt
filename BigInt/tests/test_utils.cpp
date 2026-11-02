//
// Created by artyk on 11/2/26.
//

#include <gtest/gtest.h>
#include "bigint/utils.hpp"
#include <climits>

TEST(UtilsTest, NewLine) {
	EXPECT_EQ(NLC, '\n');
	EXPECT_EQ(std::string(NLS), "\n");
}

TEST(UtilsTest, Mod0) {
	EXPECT_EQ(mod0(15,5), (15%5) == 0);
	EXPECT_EQ(mod0(15,4), (15%4) == 0);
}

TEST(UtilsTest, BoolToString) {
	EXPECT_EQ(boolToString(true), "true");
	EXPECT_EQ(boolToString(123), "true");
	EXPECT_EQ(boolToString(false), "false");
	EXPECT_EQ(boolToString(0), "false");
	
}

TEST(UtilsTest, DigitCount) {
	
	const auto digitCountValid = [](std::int64_t a)->int {
		int i = 1;
		while (a >= 10) {
			a/=10;
			i++;
		}
		return i;
	};
    
	
	for (int i = 0; i < SHRT_MAX; i++) {
		EXPECT_EQ(digitCount(i), digitCountValid(i));
	}
	
	
	
}

