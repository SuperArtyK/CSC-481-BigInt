//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <initializer_list>
#include <utility>
#include <vector>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

static_assert(noexcept(std::declval<const BigInt&>().isNormalised()));

TEST(BigintNormalisation, IsNormalisedClassifiesRepresentations) {
	struct Case {
		std::initializer_list<limb_t> limbs;
		bool negative;
		bool expectedNormalised;
	};
	const Case cases[]{
		{{0}, false, true},
		{{42}, false, true},
		{{42}, true, true},
		{{0, 42}, true, true},
		{{0}, true, false},
		{{0, 0, 0}, false, false},
		{{0, 0, 0}, true, false},
		{{123, 0, 0}, true, false},
		{{123, 0, 456, 0, 0}, false, false},
		{{STORAGE_MAX, 0, 0}, true, false},
		{{}, false, false},
		{{}, true, false},
	};

	BigInt value;
	const BigInt& constantValue = value;

	for (const Case& testCase : cases) {
		SCOPED_TRACE(
			::testing::Message()
				<< "Input limbs: "
				<< ::testing::PrintToString(std::vector<limb_t>(testCase.limbs))
				<< ", negative: " << testCase.negative
		);
		tester::load(value, testCase.limbs, testCase.negative);
		const std::vector<limb_t> expectedLimbs(testCase.limbs);

		EXPECT_EQ(constantValue.isNormalised(), testCase.expectedNormalised);
		expectRepresentation(value, expectedLimbs, testCase.negative);
	}
}

TEST(BigintNormalisation, NormaliseCanonicalizesRepresentations) {
	struct Case {
		std::initializer_list<limb_t> inputLimbs;
		bool inputNegative;
		std::initializer_list<limb_t> expectedLimbs;
		bool expectedNegative;
	};
	const Case cases[]{
		{{0}, false, {0}, false},
		{{42}, false, {42}, false},
		{{42}, true, {42}, true},
		{{0, 42}, true, {0, 42}, true},
		{{0}, true, {0}, false},
		{{0, 0, 0}, false, {0}, false},
		{{0, 0, 0}, true, {0}, false},
		{{123, 0, 0}, true, {123}, true},
		{{123, 0, 456, 0, 0}, false, {123, 0, 456}, false},
		{{STORAGE_MAX, 0, 0}, true, {STORAGE_MAX}, true},
		{{}, false, {0}, false},
		{{}, true, {0}, false},
	};

	for (const Case& testCase : cases) {
		SCOPED_TRACE(
			::testing::Message()
				<< "Input limbs: "
				<< ::testing::PrintToString(std::vector<limb_t>(testCase.inputLimbs))
				<< ", negative: " << testCase.inputNegative
		);
		BigInt value;
		tester::load(value, testCase.inputLimbs, testCase.inputNegative);

		value.normalise();

		const std::vector<limb_t> expectedLimbs(testCase.expectedLimbs);
		expectRepresentation(value, expectedLimbs, testCase.expectedNegative);
		EXPECT_TRUE(value.isNormalised());
	}
}

TEST(BigintNormalisation, NormaliseIsIdempotent) {
	BigInt value;
	tester::load(value, {123, 0, 456, 0, 0}, true);

	value.normalise();
	expectRepresentation(value, {123, 0, 456}, true);

	value.normalise();
	expectRepresentation(value, {123, 0, 456}, true);
}

TEST(BigintNormalisation, SetToZeroProducesCanonicalZero) {
	struct Case {
		std::initializer_list<limb_t> limbs;
		bool negative;
	};
	const Case inputs[]{
		{{0}, false},
		{{123, 456, 789}, false},
		{{123, 456, 789}, true},
		{{0}, true},
		{{123, 0, 0}, true},
		{{0, 0, 0}, false},
		{{0, 0, 0}, true},
		{{}, false},
		{{}, true},
	};

	for (const Case& input : inputs) {
		for (const bool shrink : {false, true}) {
			SCOPED_TRACE(
				::testing::Message()
					<< "Input limbs: "
					<< ::testing::PrintToString(std::vector<limb_t>(input.limbs))
					<< ", negative: " << input.negative
					<< ", shrink: " << shrink
			);
			BigInt value;
			tester::load(value, input.limbs, input.negative);

			value.setToZero(shrink);

			expectRepresentation(value, {0}, false);
			EXPECT_TRUE(value.isNormalised());
		}
	}
}

TEST(BigintNormalisation, SetToZeroIsIdempotent) {
	BigInt value;
	tester::load(value, {123, 456, 789}, true);

	for (int iteration = 0; iteration < 4; ++iteration) {
		const bool shrink = iteration % 2 == 0;
		SCOPED_TRACE(
			::testing::Message()
				<< "Iteration: " << iteration
				<< ", shrink: " << shrink
		);
		value.setToZero(shrink);

		expectRepresentation(value, {0}, false);
		EXPECT_TRUE(value.isNormalised());
	}
}
