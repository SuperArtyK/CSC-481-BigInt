//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;

TEST(BigintConstruction, DefaultZero) {
	BigInt a;
	
	std::vector<limb_t> limbs = tester::getLimbs(a);
	bool neg = tester::getNegative(a);
	
	ASSERT_EQ(limbs.size(), 1);
	EXPECT_EQ(limbs[0], 0);
	EXPECT_EQ(neg, false);
	
}

//Testing for carryover in BigInt addition
TEST(BigintArithmeticCarry, AddCarryAcrossLimbBoundary) {
	BigInt a;
	ASSERT_TRUE(a.setLimb(0, 999999999999999999));
	ASSERT_TRUE(a.setLimb(1, 99));

	BigInt b;
	ASSERT_TRUE(b.setLimb(0, 1));
	BigInt c = a + b;
	EXPECT_EQ(c.toStringDec(), "100000000000000000000");
}
