//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/utils.hpp"
#include <climits>

TEST(UtilsTest, DigitCount) {
	
	const auto digitCountValid = [](std::intmax_t a)->int {
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