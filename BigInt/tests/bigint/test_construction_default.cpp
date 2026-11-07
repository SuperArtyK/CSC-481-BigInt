//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "bigint_inspector.hpp"

using tester = bigint_test::Inspector;

TEST(BigintConstruction, DefaultZero) {
	BigInt a;
	
	std::vector<limb_t> limbs = tester::getLimbs(a);
	bool neg = tester::getNegative(a);
	
	ASSERT_EQ(limbs.size(), 1);
	EXPECT_EQ(limbs[0], 0);
	EXPECT_EQ(neg, false);
	
}



