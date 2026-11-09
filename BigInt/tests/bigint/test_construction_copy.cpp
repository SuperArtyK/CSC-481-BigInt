//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;


TEST(BigintConstruction, CopyZero) {
	BigInt a;
	BigInt b(a);

	expectRepresentation(b, {0}, false);
}


TEST(BigintConstruction, CopyEmpty) {
	BigInt a;
	tester::load(a, {}, false);
	BigInt b(a);

	expectRepresentation(b, {}, false);
}

TEST(BigintConstruction, CopyEmptyNegative) {
	BigInt a;

	tester::load(a, {}, true);

	BigInt b(a);

	expectRepresentation(a, {}, true);
	expectRepresentation(b, {}, true);
}

TEST(BigintConstruction, CopyValue) {
	BigInt a;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	const bool expectedNegative = true;
	tester::load(a, {123, 456, 789}, expectedNegative);

	BigInt b(a);

	expectRepresentation(a, expectedLimbs, expectedNegative);
	expectRepresentation(b, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, CopyPositiveValue) {
	BigInt a;

	const std::vector<limb_t> expected{123, 456, 789};
	tester::load(a, {123, 456, 789}, false);

	BigInt b(a);

	expectRepresentation(a, expected, false);
	expectRepresentation(b, expected, false);
}

TEST(BigintConstruction, CopyValueIndependent) {
	BigInt a;
	const std::vector<limb_t> expectedLimbs{123, 456, 789};
	const bool expectedNegative = true;
	tester::load(a, {123, 456, 789}, expectedNegative);

	BigInt b(a);

	expectRepresentation(a, expectedLimbs, expectedNegative);
	tester::load(a, {420, 999}, false);

	expectRepresentation(a, {420, 999}, false);
	expectRepresentation(b, expectedLimbs, expectedNegative);
}

TEST(BigintConstruction, CopyValueDenorm_TrailZero) {
	BigInt a;
	const std::vector<limb_t> expectedLimbs{123, 456, 789, 0, 0, 0, 0};
	const bool expectedNegative = true;
	tester::load(a, {123, 456, 789, 0, 0, 0, 0}, expectedNegative);

	BigInt b(a);

	expectRepresentation(a, expectedLimbs, expectedNegative);
	expectRepresentation(b, expectedLimbs, expectedNegative);
}


TEST(BigintConstruction, CopyValueDenorm_ZeroNeg) {
	BigInt a;
	tester::load(a, {0}, true);

	BigInt b(a);
	expectRepresentation(a, {0}, true);
	expectRepresentation(b, {0}, true);
}

TEST(BigintConstruction, CopyValueDenorm_ZeroTrailNeg) {
	BigInt a;
	const std::vector<limb_t> expectedLimbs{0, 0, 0, 0, 0, 0};
	const bool expectedNegative = true;
	tester::load(a, {0, 0, 0, 0, 0, 0}, expectedNegative);

	BigInt b(a);
	expectRepresentation(a, expectedLimbs, expectedNegative);
	expectRepresentation(b, expectedLimbs, expectedNegative);
}
