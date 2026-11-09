//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <type_traits>
#include <utility>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;
using bigint_test::testMovePreservesRepresentation;

static_assert(std::is_nothrow_assignable_v<BigInt&, BigInt&&>);

TEST(BigintAssignment, MoveCanonicalZero) {
	BigInt source;
	BigInt destination;
	tester::load(destination, {42, 0, 0}, true);

	BigInt& result = (destination = std::move(source));

	EXPECT_EQ(&result, &destination);
	expectRepresentation(destination, {0}, false);
}

TEST(BigintAssignment, MovePositiveValue) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, false);

	destination = std::move(source);

	expectRepresentation(destination, {123, 456, 789}, false);
}

TEST(BigintAssignment, MoveNegativeValue) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, true);

	destination = std::move(source);

	expectRepresentation(destination, {123, 456, 789}, true);
}

TEST(BigintAssignment, MoveSourceLargerThanDestination) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, false);
	tester::load(destination, {42}, true);

	destination = std::move(source);

	expectRepresentation(destination, {123, 456, 789}, false);
}

TEST(BigintAssignment, MoveSourceSmallerThanDestination) {
	BigInt source;
	BigInt destination;
	tester::load(source, {42}, true);
	tester::load(destination, {123, 456, 789}, false);

	destination = std::move(source);

	expectRepresentation(destination, {42}, true);
}

TEST(BigintAssignment, MovePreservesRedundantZeroLimbs) {
	testMovePreservesRepresentation({123, 456, 0, 0}, true);
	testMovePreservesRepresentation({0, 0, 0}, false);
}

TEST(BigintAssignment, MovePreservesNegativeZero) {
	testMovePreservesRepresentation({0}, true);
}

TEST(BigintAssignment, MovePreservesEmptyStorageWithBothSigns) {
	testMovePreservesRepresentation({}, false);
	testMovePreservesRepresentation({}, true);
}

TEST(BigintAssignment, MoveIgnoresDestinationsPreviousNormalization) {
	BigInt normalizedSource;
	BigInt nonNormalizedDestination;
	tester::load(normalizedSource, {123, 456}, false);
	tester::load(nonNormalizedDestination, {42, 0, 0}, true);

	nonNormalizedDestination = std::move(normalizedSource);
	expectRepresentation(nonNormalizedDestination, {123, 456}, false);

	BigInt nonNormalizedSource;
	BigInt normalizedDestination;
	tester::load(nonNormalizedSource, {789, 0, 0}, true);
	tester::load(normalizedDestination, {42}, false);

	normalizedDestination = std::move(nonNormalizedSource);
	expectRepresentation(normalizedDestination, {789, 0, 0}, true);
}

TEST(BigintAssignment, MoveDestinationIsIndependentOfRestoredSource) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 0}, true);

	destination = std::move(source);
	expectRepresentation(destination, {123, 456, 0}, true);

	source.setToZero();

	expectRepresentation(source, {0}, false);
	expectRepresentation(destination, {123, 456, 0}, true);
}

TEST(BigintAssignment, MoveSourceRemainsCopyableAssignableAndRestorable) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, true);

	destination = std::move(source);

	BigInt sourceCopy(source);
	expectRepresentation(sourceCopy, tester::getLimbs(source), tester::getNegative(source));

	BigInt assignedSource;
	assignedSource = source;
	expectRepresentation(
		assignedSource,
		tester::getLimbs(source),
		tester::getNegative(source)
	);

	source.setToZero();
	expectRepresentation(source, {0}, false);

	source = destination;
	expectRepresentation(source, {123, 456, 789}, true);
	expectRepresentation(destination, {123, 456, 789}, true);
}

TEST(BigintAssignment, MoveSelfAssignmentPreservesRepresentation) {
	BigInt value;
	tester::load(value, {}, true);

	BigInt& result = (value = std::move(value));

	EXPECT_EQ(&result, &value);
	expectRepresentation(value, {}, true);
}

TEST(BigintAssignment, MoveSelfAssignmentPreservesMultiLimbRepresentation) {
	BigInt value;
	tester::load(value, {123, 456, 789}, true);

	BigInt& result = (value = std::move(value));

	EXPECT_EQ(&result, &value);
	expectRepresentation(value, {123, 456, 789}, true);
}