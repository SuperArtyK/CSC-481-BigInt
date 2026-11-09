//
// Created by artyk on 11/9/26.
//
#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;
using bigint_test::testCopyPreservesRepresentation;

TEST(BigintAssignment, CopyCanonicalZero) {
	BigInt source;
	BigInt destination;
	tester::load(destination, {42, 0, 0}, true);

	BigInt& result = (destination = source);

	EXPECT_EQ(&result, &destination);
	expectRepresentation(destination, {0}, false);
}

TEST(BigintAssignment, CopyPositiveValue) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, false);

	destination = source;

	expectRepresentation(destination, {123, 456, 789}, false);
}

TEST(BigintAssignment, CopyNegativeValue) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, true);

	destination = source;

	expectRepresentation(destination, {123, 456, 789}, true);
}

TEST(BigintAssignment, CopySourceLargerThanDestination) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 789}, false);
	tester::load(destination, {42}, true);

	destination = source;

	expectRepresentation(destination, {123, 456, 789}, false);
}

TEST(BigintAssignment, CopySourceSmallerThanDestination) {
	BigInt source;
	BigInt destination;
	tester::load(source, {42}, true);
	tester::load(destination, {123, 456, 789}, false);

	destination = source;

	expectRepresentation(destination, {42}, true);
}

TEST(BigintAssignment, CopyPreservesRedundantZeroLimbs) {
	testCopyPreservesRepresentation({123, 456, 0, 0}, true);
	testCopyPreservesRepresentation({0, 0, 0}, false);
}

TEST(BigintAssignment, CopyPreservesNegativeZero) {
	testCopyPreservesRepresentation({0}, true);
}

TEST(BigintAssignment, CopyPreservesEmptyStorageWithBothSigns) {
	testCopyPreservesRepresentation({}, false);
	testCopyPreservesRepresentation({}, true);
}

TEST(BigintAssignment, CopyIgnoresDestinationsPreviousNormalization) {
	BigInt normalizedSource;
	BigInt nonNormalizedDestination;
	tester::load(normalizedSource, {123, 456}, false);
	tester::load(nonNormalizedDestination, {42, 0, 0}, true);

	nonNormalizedDestination = normalizedSource;
	expectRepresentation(nonNormalizedDestination, {123, 456}, false);

	BigInt nonNormalizedSource;
	BigInt normalizedDestination;
	tester::load(nonNormalizedSource, {789, 0, 0}, true);
	tester::load(normalizedDestination, {42}, false);

	normalizedDestination = nonNormalizedSource;
	expectRepresentation(normalizedDestination, {789, 0, 0}, true);
}

TEST(BigintAssignment, CopyPreservesSourceAndUsesIndependentStorage) {
	BigInt source;
	BigInt destination;
	tester::load(source, {123, 456, 0}, true);

	destination = source;
	expectRepresentation(source, {123, 456, 0}, true);
	expectRepresentation(destination, {123, 456, 0}, true);

	tester::load(source, {42, 999}, false);

	expectRepresentation(source, {42, 999}, false);
	expectRepresentation(destination, {123, 456, 0}, true);
}

TEST(BigintAssignment, CopySelfAssignmentPreservesRepresentation) {
	BigInt value;
	tester::load(value, {123, 456, 0, 0}, true);

	BigInt& result = (value = value);

	EXPECT_EQ(&result, &value);
	expectRepresentation(value, {123, 456, 0, 0}, true);
}