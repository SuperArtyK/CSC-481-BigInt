//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <limits>
#include <type_traits>
#include <utility>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

static_assert(noexcept(std::declval<const BigInt&>().getLimbCount()));
static_assert(noexcept(std::declval<const BigInt&>().getLimb(0)));
static_assert(noexcept(std::declval<const BigInt&>().getFirstLimb()));
static_assert(noexcept(std::declval<const BigInt&>().getLastLimb()));
static_assert(noexcept(std::declval<const BigInt&>().isNegative()));
static_assert(noexcept(std::declval<const BigInt&>().isZero()));
static_assert(noexcept(std::declval<const BigInt&>().getDigitCount()));

TEST(BigintAccessors, GetLimbCount) {
	BigInt value;
	const BigInt& constantValue = value;

	EXPECT_EQ(constantValue.getLimbCount(), 1u);

	tester::load(value, {42}, false);
	EXPECT_EQ(constantValue.getLimbCount(), 1u);

	tester::load(value, {123, 456, 789}, true);
	EXPECT_EQ(constantValue.getLimbCount(), 3u);

	tester::load(value, {123, 0, 0, 0}, false);
	EXPECT_EQ(constantValue.getLimbCount(), 4u);

	tester::load(value, {}, true);
	EXPECT_EQ(constantValue.getLimbCount(), 0u);
}

TEST(BigintAccessors, GetLimb) {
	BigInt value;
	tester::load(value, {123, 0, 789, 0}, true);
	const BigInt& constantValue = value;

	EXPECT_EQ(constantValue.getLimb(0), 123u);
	EXPECT_EQ(constantValue.getLimb(1), 0u);
	EXPECT_EQ(constantValue.getLimb(2), 789u);
	EXPECT_EQ(constantValue.getLimb(3), 0u);

	tester::load(value, {STORAGE_MAX}, false);
	EXPECT_EQ(constantValue.getLimb(0), STORAGE_MAX);

	EXPECT_EQ(constantValue.getLimb(4), 0u);
	EXPECT_EQ(constantValue.getLimb(100), 0u);
	EXPECT_EQ(
		constantValue.getLimb(std::numeric_limits<std::size_t>::max()),
		0u
	);

	tester::load(value, {}, false);
	EXPECT_EQ(constantValue.getLimb(0), 0u);
	EXPECT_EQ(constantValue.getLimb(100), 0u);
}

TEST(BigintAccessors, GetFirstLimb) {
	BigInt value;
	tester::load(value, {42}, false);
	const BigInt& constantValue = value;
	EXPECT_EQ(constantValue.getFirstLimb(), 42u);

	tester::load(value, {0, 456, 789}, true);
	EXPECT_EQ(constantValue.getFirstLimb(), 0u);
}

TEST(BigintAccessors, GetLastLimb) {
	BigInt value;
	tester::load(value, {42}, false);
	const BigInt& constantValue = value;
	EXPECT_EQ(constantValue.getLastLimb(), 42u);

	tester::load(value, {123, 456, 789}, true);
	EXPECT_EQ(constantValue.getLastLimb(), 789u);

	tester::load(value, {123, 456, 0, 0}, false);
	EXPECT_EQ(constantValue.getLastLimb(), 0u);
}

TEST(BigintAccessors, IsNegative) {
	BigInt value;
	const BigInt& constantValue = value;

	EXPECT_FALSE(constantValue.isNegative());

	tester::load(value, {1}, false);
	EXPECT_FALSE(constantValue.isNegative());

	tester::load(value, {1}, true);
	EXPECT_TRUE(constantValue.isNegative());

	tester::load(value, {0}, false);
	EXPECT_FALSE(constantValue.isNegative());

	tester::load(value, {0}, true);
	EXPECT_TRUE(constantValue.isNegative());

	tester::load(value, {}, false);
	EXPECT_FALSE(constantValue.isNegative());

	tester::load(value, {}, true);
	EXPECT_TRUE(constantValue.isNegative());
}

TEST(BigintAccessors, IsZero) {
	BigInt value;
	const BigInt& constantValue = value;

	tester::load(value, {0}, false);
	EXPECT_TRUE(constantValue.isZero());

	tester::load(value, {1}, false);
	EXPECT_FALSE(constantValue.isZero());

	tester::load(value, {1}, true);
	EXPECT_FALSE(constantValue.isZero());

	tester::load(value, {0, 1}, false);
	EXPECT_FALSE(constantValue.isZero());

	tester::load(value, {0, 1}, true);
	EXPECT_FALSE(constantValue.isZero());
}

TEST(BigintAccessors, GetDigitCount) {
	BigInt value;
	const BigInt& constantValue = value;

	tester::load(value, {0}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 1u);

	tester::load(value, {1}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 1u);

	tester::load(value, {9}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 1u);

	tester::load(value, {10}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 2u);

	tester::load(value, {99}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 2u);

	tester::load(value, {100}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 3u);

	tester::load(value, {999}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 3u);

	tester::load(value, {1000}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 4u);

	tester::load(value, {STORAGE_MAX}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 18u);

	tester::load(value, {0, 1}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 19u);

	tester::load(value, {STORAGE_MAX, 1}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 19u);

	tester::load(value, {0, 10}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 20u);

	tester::load(value, {0, STORAGE_MAX}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 36u);

	tester::load(value, {0, 0, 1}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 37u);

	tester::load(value, {0, 0, STORAGE_MAX}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 54u);

	tester::load(value, {0, 0, STORAGE_MAX}, true);
	EXPECT_EQ(constantValue.getDigitCount(), 54u);
}

TEST(BigintAccessors, GetDigitCountTopLimbBoundary) {
	BigInt value;
	const BigInt& constantValue = value;

	tester::load(value, {0, STORAGE_MAX / 10}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 35u);

	tester::load(value, {0, STORAGE_MAX / 10 + 1}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 36u);

	tester::load(value, {0, STORAGE_MAX}, false);
	EXPECT_EQ(constantValue.getDigitCount(), 36u);
}

TEST(BigintAccessors, AccessorsDoNotMutateStorage) {
	BigInt value;
	tester::load(value, {123, 0, 789, 0}, true);
	const BigInt& constantValue = value;
	const std::vector<limb_t> expectedLimbs{123, 0, 789, 0};

	EXPECT_EQ(constantValue.getLimbCount(), 4u);
	EXPECT_EQ(constantValue.getLimb(2), 789u);
	EXPECT_EQ(constantValue.getFirstLimb(), 123u);
	EXPECT_EQ(constantValue.getLastLimb(), 0u);
	EXPECT_TRUE(constantValue.isNegative());

	expectRepresentation(value, expectedLimbs, true);
}

TEST(BigintAccessors, NumericalQueriesDoNotMutateStorage) {
	BigInt value;
	tester::load(value, {0, 123}, true);
	const BigInt& constantValue = value;

	EXPECT_FALSE(constantValue.isZero());
	EXPECT_EQ(constantValue.getDigitCount(), 21u);
	expectRepresentation(value, {0, 123}, true);

	tester::load(value, {0}, false);

	EXPECT_TRUE(constantValue.isZero());
	EXPECT_EQ(constantValue.getDigitCount(), 1u);
	expectRepresentation(value, {0}, false);
}
