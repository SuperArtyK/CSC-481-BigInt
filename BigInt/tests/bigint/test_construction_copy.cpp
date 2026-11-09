//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;


TEST(BigintConstruction, CopyZero) {
	BigInt a;
	BigInt b(a);
	
	std::vector<limb_t> limbs = tester::getLimbs(b);
	bool neg = tester::getNegative(b);
	
	ASSERT_EQ(limbs.size(), 1);
	EXPECT_EQ(limbs[0], 0);
	EXPECT_EQ(neg, false);
}


TEST(BigintConstruction, CopyEmpty) {
	BigInt a;
	tester::load(a, {}, false);
	BigInt b(a);
	
	EXPECT_TRUE(tester::getLimbs(b).empty());
	EXPECT_EQ(tester::getNegative(b), false);
}

TEST(BigintConstruction, CopyEmptyNegative) {
	BigInt a;

	tester::load(a, {}, true);

	BigInt b(a);

	EXPECT_TRUE(tester::getLimbs(a).empty());
	EXPECT_TRUE(tester::getNegative(a));

	EXPECT_TRUE(tester::getLimbs(b).empty());
	EXPECT_TRUE(tester::getNegative(b));
}

TEST(BigintConstruction, CopyValue) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	std::vector<limb_t> limbsA = tester::getLimbs(a);
	
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(a);
	//test that copy doesn't mutate in the first place
	limbsA = tester::getLimbs(a);
	ASSERT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	ASSERT_EQ(tester::getNegative(a), expNeg);
	
	
	
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, limbsA);
	EXPECT_EQ(tester::getNegative(b), tester::getNegative(a));
	
}

TEST(BigintConstruction, CopyPositiveValue) {
	BigInt a;

	const std::vector<limb_t> expected{123, 456, 789};
	tester::load(a, {123, 456, 789}, false);

	BigInt b(a);

	// The source must remain unchanged.
	EXPECT_EQ(tester::getLimbs(a), expected);
	EXPECT_FALSE(tester::getNegative(a));

	// The destination must preserve the representation.
	EXPECT_EQ(tester::getLimbs(b), expected);
	EXPECT_FALSE(tester::getNegative(b));
}

TEST(BigintConstruction, CopyValueIndependent) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	
	BigInt b(a);
	
	//test that copy doesn't mutate in the first place
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	ASSERT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	ASSERT_EQ(tester::getNegative(a), expNeg);
	
	
	tester::load(a, {420,999}, false);
	
	
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_NE(limbsB, tester::getLimbs(a));
	EXPECT_NE(tester::getNegative(b), tester::getNegative(a));
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
	
}

TEST(BigintConstruction, CopyValueDenorm_TrailZero) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789, 0,0,0,0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(a);
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
	
}


TEST(BigintConstruction, CopyValueDenorm_ZeroNeg) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(a);
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
}

TEST(BigintConstruction, CopyValueDenorm_ZeroTrailNeg) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {0,0,0,0,0,0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(a);
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
}
