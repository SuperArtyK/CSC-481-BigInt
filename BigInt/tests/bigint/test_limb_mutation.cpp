//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <limits>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintSetNegative, ChangesSignWithoutNormalising) {
	BigInt value;
	tester::load(value, {0, 0, 0}, false);

	value.setNegative(true);
	expectRepresentation(value, {0, 0, 0}, true);

	value.setNegative(false);
	expectRepresentation(value, {0, 0, 0}, false);

	tester::load(value, {}, false);
	value.setNegative(true);
	expectRepresentation(value, {}, true);
}

TEST(BigintSetNegative, ChangesSignForNonzeroAndCanonicalZero) {
	BigInt value;
	tester::load(value, {123, 456}, false);

	value.setNegative(true);
	expectRepresentation(value, {123, 456}, true);

	value.setNegative(false);
	expectRepresentation(value, {123, 456}, false);

	tester::load(value, {0}, false);
	value.setNegative(true);
	expectRepresentation(value, {0}, true);
}

TEST(BigintSetLimbRaw, ReplacesExistingLimbAndPreservesSign) {
	BigInt value;
	tester::load(value, {123, 456, 789}, true);

	ASSERT_TRUE(value.setLimbRaw(1, 42));

	expectRepresentation(value, {123, 42, 789}, true);
}

TEST(BigintSetLimbRaw, AcceptsMaximumLimbValue) {
	BigInt value;
	tester::load(value, {1}, false);

	ASSERT_TRUE(value.setLimbRaw(0, STORAGE_MAX));

	expectRepresentation(value, {STORAGE_MAX}, false);
}

TEST(BigintSetLimbRaw, RejectsInvalidValueWithoutMutation) {
	BigInt value;
	tester::load(value, {123, 456, 0, 0}, true);

	EXPECT_FALSE(value.setLimbRaw(1, STORAGE_MASK));

	expectRepresentation(value, {123, 456, 0, 0}, true);
}

TEST(BigintSetLimbRaw, InvalidValueDoesNotExtendStorage) {
	BigInt value;
	tester::load(value, {123, 0, 0}, true);

	EXPECT_FALSE(value.setLimbRaw(10, STORAGE_MASK));

	expectRepresentation(value, {123, 0, 0}, true);
}

TEST(BigintSetLimbRaw, ExtendsStorageWithNonzeroValueAndZeroFills) {
	BigInt value;
	tester::load(value, {123}, true);

	ASSERT_TRUE(value.setLimbRaw(3, 42));

	expectRepresentation(value, {123, 0, 0, 42}, true);
}

TEST(BigintSetLimbRaw, ExtendsStorageWithZero) {
	BigInt value;
	tester::load(value, {123}, true);

	ASSERT_TRUE(value.setLimbRaw(3, 0));

	expectRepresentation(value, {123, 0, 0, 0}, true);
}

TEST(BigintSetLimbRaw, PreservesRedundantZeroAndNegativeZero) {
	BigInt value;
	tester::load(value, {123, 456}, true);

	ASSERT_TRUE(value.setLimbRaw(1, 0));
	expectRepresentation(value, {123, 0}, true);

	tester::load(value, {0}, true);
	ASSERT_TRUE(value.setLimbRaw(0, 0));
	expectRepresentation(value, {0}, true);
}

TEST(BigintSetLimbRaw, ExtendsEmptyStorageWithoutNormalizing) {
	BigInt value;
	tester::load(value, {}, true);

	ASSERT_TRUE(value.setLimbRaw(2, 0));

	expectRepresentation(value, {0, 0, 0}, true);
}

TEST(BigintSetLimbRaw, NonzeroWriteToNegativeEmptyStorage) {
	BigInt value;
	tester::load(value, {}, true);

	ASSERT_TRUE(value.setLimbRaw(2, 42));

	expectRepresentation(value, {0, 0, 42}, true);
}

TEST(BigintSetLimbRaw, MaximumIndexThrowsInsteadOfOverflowing) {
	BigInt value;
	const std::size_t maximum = std::numeric_limits<std::size_t>::max();

	EXPECT_ANY_THROW(value.setLimbRaw(maximum, 1));
}

TEST(BigintSetLimbRaw, MaximumIndexZeroThrows) {
	BigInt value;
	const std::size_t maximum = std::numeric_limits<std::size_t>::max();

	EXPECT_ANY_THROW(value.setLimbRaw(maximum, 0));
}

TEST(BigintSetLimbRaw, MaximumIndexInvalidValueReturnsFalse) {
	BigInt value;
	tester::load(value, {123, 456}, true);
	const std::size_t maximum = std::numeric_limits<std::size_t>::max();

	EXPECT_FALSE(value.setLimbRaw(maximum, STORAGE_MASK));

	expectRepresentation(value, {123, 456}, true);
}

TEST(BigintSetLimb, ReplacesExistingLimbAndPreservesSign) {
	BigInt value;
	tester::load(value, {123, 456, 789}, true);

	ASSERT_TRUE(value.setLimb(1, 42));

	expectRepresentation(value, {123, 42, 789}, true);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, UnchangedValueStillNormalizes) {
	BigInt value;
	tester::load(value, {123, 0, 0}, true);

	ASSERT_TRUE(value.setLimb(0, 123));

	expectRepresentation(value, {123}, true);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, NormalizesAfterZeroingMostSignificantLimb) {
	BigInt value;
	tester::load(value, {123, 456, 789}, true);

	ASSERT_TRUE(value.setLimb(2, 0));

	expectRepresentation(value, {123, 456}, true);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, ZeroesOnlyLimbToCanonicalZero) {
	BigInt value;
	tester::load(value, {42}, true);

	ASSERT_TRUE(value.setLimb(0, 0));

	expectRepresentation(value, {0}, false);
	EXPECT_TRUE(value.isNormalised());

	tester::load(value, {0}, true);

	ASSERT_TRUE(value.setLimb(0, 0));

	expectRepresentation(value, {0}, false);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, ExtendsStorageWithNonzeroValueAndZeroFills) {
	BigInt value;
	tester::load(value, {123}, false);

	ASSERT_TRUE(value.setLimb(3, 42));

	expectRepresentation(value, {123, 0, 0, 42}, false);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, OutOfRangeZeroIsCompleteNoOp) {
	BigInt value;
	tester::load(value, {123, 0, 0}, true);

	ASSERT_TRUE(value.setLimb(5, 0));
	expectRepresentation(value, {123, 0, 0}, true);

	ASSERT_TRUE(value.setLimb(1, 0));
	expectRepresentation(value, {123}, true);
}

TEST(BigintSetLimb, MaximumIndexZeroIsNoOp) {
	BigInt value;
	tester::load(value, {123, 0, 0}, true);

	EXPECT_TRUE(value.setLimb(std::numeric_limits<std::size_t>::max(), 0));

	expectRepresentation(value, {123, 0, 0}, true);
}

TEST(BigintSetLimb, MaximumIndexNonzeroThrows) {
	BigInt value;

	EXPECT_ANY_THROW(
		value.setLimb(std::numeric_limits<std::size_t>::max(), 1)
	);
}

TEST(BigintSetLimb, RejectsInvalidValueWithoutMutation) {
	BigInt value;
	tester::load(value, {123, 456, 0}, true);

	EXPECT_FALSE(value.setLimb(1, STORAGE_MASK));

	expectRepresentation(value, {123, 456, 0}, true);
}

TEST(BigintSetLimb, InvalidValueDoesNotExtendStorage) {
	BigInt value;
	tester::load(value, {123, 0, 0}, true);

	EXPECT_FALSE(value.setLimb(10, STORAGE_MASK));

	expectRepresentation(value, {123, 0, 0}, true);
}

TEST(BigintSetLimb, AcceptsMaximumLimbValue) {
	BigInt value;
	tester::load(value, {0}, false);

	ASSERT_TRUE(value.setLimb(0, STORAGE_MAX));

	expectRepresentation(value, {STORAGE_MAX}, false);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, NonzeroWriteToNegativeZeroPreservesNegativeSign) {
	BigInt value;
	tester::load(value, {0}, true);

	ASSERT_TRUE(value.setLimb(2, 42));

	expectRepresentation(value, {0, 0, 42}, true);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, ZeroResultClearsNegativeSign) {
	BigInt value;
	tester::load(value, {0, 0, 42}, true);

	ASSERT_TRUE(value.setLimb(2, 0));

	expectRepresentation(value, {0}, false);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, NonzeroWriteToEmptyStorageExtendsAndNormalizes) {
	BigInt value;
	tester::load(value, {}, false);

	ASSERT_TRUE(value.setLimb(2, 42));

	expectRepresentation(value, {0, 0, 42}, false);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, NonzeroWriteToNegativeEmptyStorage) {
	BigInt value;
	tester::load(value, {}, true);

	ASSERT_TRUE(value.setLimb(2, 42));

	expectRepresentation(value, {0, 0, 42}, true);
	EXPECT_TRUE(value.isNormalised());
}

TEST(BigintSetLimb, ZeroWriteToEmptyStorageIsNoOp) {
	BigInt value;
	tester::load(value, {}, true);

	ASSERT_TRUE(value.setLimb(0, 0));

	expectRepresentation(value, {}, true);
}
