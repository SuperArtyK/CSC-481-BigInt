//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include <utility>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintConstruction, MoveZero) {
	BigInt source;
	BigInt destination(std::move(source));

	expectRepresentation(destination, {0}, false);
}

TEST(BigintConstruction, MoveEmpty) {
	BigInt source;
	tester::load(source, {}, false);

	BigInt destination(std::move(source));

	expectRepresentation(destination, {}, false);
}

TEST(BigintConstruction, MoveEmptyNegative) {
	BigInt source;
	tester::load(source, {}, true);

	BigInt destination(std::move(source));

	expectRepresentation(destination, {}, true);
}

TEST(BigintConstruction, MoveValue) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	const bool expectedNegative = true;
	tester::load(source, {123, 456, 789}, expectedNegative);

	BigInt destination(std::move(source));

	expectRepresentation(destination, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, MovePositiveValue) {
	BigInt source;
	tester::load(source, {123, 456, 789}, false);

	BigInt destination(std::move(source));

	expectRepresentation(destination, {123, 456, 789}, false);
}

TEST(BigintConstruction, MoveValueIndependent) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	const bool expectedNegative = true;
	tester::load(source, {123, 456, 789}, expectedNegative);

	BigInt destination(std::move(source));
	tester::load(source, {420, 999}, false);

	expectRepresentation(source, {420, 999}, false);
	expectRepresentation(destination, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, MoveValueDenorm_TrailZero) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{123, 456, 789, 0, 0, 0, 0};
	const bool expectedNegative = true;
	tester::load(source, {123, 456, 789, 0, 0, 0, 0}, expectedNegative);

	BigInt destination(std::move(source));

	expectRepresentation(destination, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, MoveValueDenorm_ZeroNeg) {
	BigInt source;
	tester::load(source, {0}, true);

	BigInt destination(std::move(source));

	expectRepresentation(destination, {0}, true);
}

TEST(BigintConstruction, MoveValueDenorm_ZeroTrailNeg) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{0, 0, 0, 0, 0, 0};
	const bool expectedNegative = true;
	tester::load(source, {0, 0, 0, 0, 0, 0}, expectedNegative);

	BigInt destination(std::move(source));

	expectRepresentation(destination, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, MoveSource_Restored) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	tester::load(source, {123, 456, 789}, true);

	BigInt destination(std::move(source));
	expectRepresentation(destination, expectedLimbs, true);

	source.setToZero();

	expectRepresentation(source, {0}, false);
	expectRepresentation(destination, expectedLimbs, true);
}

TEST(BigintConstruction, MoveSource_Assigned) {
	BigInt source;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	tester::load(source, {123, 456, 789}, true);

	BigInt destination(std::move(source));
	expectRepresentation(destination, expectedLimbs, true);

	source = destination;

	expectRepresentation(source, expectedLimbs, true);
	expectRepresentation(destination, expectedLimbs, true);

	tester::load(destination, {42, 999}, false);

	expectRepresentation(source, expectedLimbs, true);
}
