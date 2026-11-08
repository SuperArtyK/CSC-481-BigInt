//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "bigint_inspector.hpp"

using tester = bigint_test::Inspector;


TEST(BigintConstruction, MoveZero) {
	BigInt a;
	BigInt b(std::move(a));
	
	std::vector<limb_t> limbs = tester::getLimbs(b);
	bool neg = tester::getNegative(b);
	
	ASSERT_EQ(limbs.size(), 1);
	EXPECT_EQ(limbs[0], 0);
	EXPECT_EQ(neg, false);
}

TEST(BigintConstruction, MoveEmpty) {
	BigInt a;
	tester::load(a, {}, false);
	BigInt b(std::move(a));
	
	EXPECT_TRUE(tester::getLimbs(b).empty());
	EXPECT_EQ(tester::getNegative(b), false);
}

TEST(BigintConstruction, MoveEmptyNegative) {
	BigInt a;

	tester::load(a, {}, true);

	BigInt b(std::move(a));

	EXPECT_TRUE(tester::getLimbs(b).empty());
	EXPECT_TRUE(tester::getNegative(b));
}

TEST(BigintConstruction, MoveValue) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(std::move(a));
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
	
}

TEST(BigintConstruction, MovePositiveValue) {
	BigInt a;

	tester::load(a, {123, 456, 789}, false);

	BigInt b(std::move(a));

	EXPECT_EQ(
		tester::getLimbs(b),
		(std::vector<limb_t>{123, 456, 789})
	);
	EXPECT_FALSE(tester::getNegative(b));
}

TEST(BigintConstruction, MoveValueIndependent) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	
	BigInt b(std::move(a));
	
	tester::load(a, {420,999}, false);
	
	
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_NE(limbsB, tester::getLimbs(a));
	EXPECT_NE(tester::getNegative(b), tester::getNegative(a));
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
	
}


TEST(BigintConstruction, MoveValueDenorm_TrailZero) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {123,456,789, 0,0,0,0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(std::move(a));
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
	
}


TEST(BigintConstruction, MoveValueDenorm_ZeroNeg) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(std::move(a));
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
}

TEST(BigintConstruction, MoveValueDenorm_ZeroTrailNeg) {
	BigInt a;
	
	const std::initializer_list<limb_t> expLimbs {0,0,0,0,0,0};
	const bool expNeg = true;
	
	tester::load(a, expLimbs, expNeg);
	const std::vector<limb_t> limbsA = tester::getLimbs(a);
	EXPECT_EQ(limbsA, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(a), expNeg);
	
	BigInt b(std::move(a));
	const std::vector<limb_t> limbsB = tester::getLimbs(b);
	EXPECT_EQ(limbsB, std::vector<limb_t>(expLimbs));
	EXPECT_EQ(tester::getNegative(b), expNeg);
}

TEST(BigintConstruction, MoveSource_Restored) {
	BigInt a;

	const std::vector<limb_t> expected{123, 456, 789};
	tester::load(a, {123, 456, 789}, true);

	BigInt b(std::move(a));

	// Verify the destination.
	EXPECT_EQ(tester::getLimbs(b), expected);
	EXPECT_TRUE(tester::getNegative(b));

	// Restore the moved-from source through the public API.
	a.setToZero();

	// Source must now contain canonical zero.
	EXPECT_EQ(
		tester::getLimbs(a),
		(std::vector<limb_t>{0})
	);
	EXPECT_FALSE(tester::getNegative(a));

	// Restoring the source must not affect the destination.
	EXPECT_EQ(tester::getLimbs(b), expected);
	EXPECT_TRUE(tester::getNegative(b));
}

TEST(BigintConstruction, MoveSource_Assigned) {
	BigInt a;

	// Establish the original representation.
	tester::load(a, {123, 456, 789}, true);

	// Move from a into b.
	BigInt b(std::move(a));

	// Verify the move destination.
	const std::vector<limb_t> expected{123, 456, 789};

	EXPECT_EQ(tester::getLimbs(b), expected);
	EXPECT_TRUE(tester::getNegative(b));

	// The moved-from object must remain assignable.
	a = b;

	// Verify assignment restored the expected representation.
	EXPECT_EQ(tester::getLimbs(a), expected);
	EXPECT_TRUE(tester::getNegative(a));

	// Assignment must not modify the source.
	EXPECT_EQ(tester::getLimbs(b), expected);
	EXPECT_TRUE(tester::getNegative(b));

	// Verify independence after assignment.
	tester::load(b, {42, 999}, false);

	EXPECT_EQ(tester::getLimbs(a), expected);
	EXPECT_TRUE(tester::getNegative(a));
}