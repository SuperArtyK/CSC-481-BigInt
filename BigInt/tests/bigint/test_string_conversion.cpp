//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <vector>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintStringConversion, ToStringDecExactValues) {
	struct Case {
		std::initializer_list<limb_t> limbs;
		bool negative;
		std::string expected;
	};
	const Case cases[]{
		{{0}, false, "0"},
		{{1}, false, "1"},
		{{9}, false, "9"},
		{{10}, false, "10"},
		{{99}, false, "99"},
		{{100}, false, "100"},
		{{STORAGE_MAX}, false, std::string(18, '9')},
		{{STORAGE_MAX}, true, "-" + std::string(18, '9')},
		{{1}, true, "-1"},
		{{42}, true, "-42"},
		{{0, 1}, false, "1" + std::string(18, '0')},
		{{1, 1}, false, "1" + std::string(17, '0') + "1"},
		{{42, 1}, false, "1" + std::string(16, '0') + "42"},
		{
			{1, 99999999999999999ULL},
			false,
			"99999999999999999" + std::string(17, '0') + "1"
		},
		{{0, 10}, false, "10" + std::string(18, '0')},
		{{STORAGE_MAX, 1}, false, "1" + std::string(18, '9')},
		{
			{123456789012345678ULL, 987654321098765432ULL},
			false,
			"987654321098765432123456789012345678"
		},
		{{0, 0, 1}, false, "1" + std::string(36, '0')},
		{
			{123456789012345678ULL, 0, 987654321098765432ULL},
			true,
			"-987654321098765432"
				+ std::string(18, '0')
				+ "123456789012345678"
		},
		{{STORAGE_MAX, STORAGE_MAX, STORAGE_MAX}, false, std::string(54, '9')},
	};

	for (const Case& testCase : cases) {
		SCOPED_TRACE(
			::testing::Message()
				<< "Input limbs: "
				<< ::testing::PrintToString(std::vector<limb_t>(testCase.limbs))
				<< ", negative: " << testCase.negative
		);

		BigInt value;
		tester::load(value, testCase.limbs, testCase.negative);
		const BigInt& constantValue = value;

		EXPECT_EQ(constantValue.toStringDec(), testCase.expected);
		expectRepresentation(
			value,
			std::vector<limb_t>(testCase.limbs),
			testCase.negative
		);
	}
}

TEST(BigintStringConversion, ToStringDecNeverTruncates) {
	BigInt value;
	const std::initializer_list<limb_t> limbs{
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
		STORAGE_MAX,
	};
	tester::load(value, limbs, true);
	const BigInt& constantValue = value;
	const std::string expected = "-" + std::string(144, '9');

	EXPECT_EQ(constantValue.toStringDec(), expected);
	EXPECT_EQ(constantValue.toStringDec().size(), expected.size());
	expectRepresentation(value, std::vector<limb_t>(limbs), true);
}

TEST(BigintStringConversion, ToStringDecPreservesInternalZeroLimbs) {
	BigInt value;
	tester::load(value, {42, 0, 0, 0, 1}, false);
	const BigInt& constantValue = value;
	const std::string expected = "1" + std::string(70, '0') + "42";

	EXPECT_EQ(constantValue.toStringDec(), expected);
	expectRepresentation(value, {42, 0, 0, 0, 1}, false);
}

TEST(BigintStringConversion, ToStringDecRepeatedFormattingIsStable) {
	BigInt value;
	tester::load(value, {123456789012345678ULL, 0, STORAGE_MAX}, true);
	const BigInt& constantValue = value;
	const std::string expected =
		"-" + std::string(18, '9') + std::string(18, '0') + "123456789012345678";

	EXPECT_EQ(constantValue.toStringDec(), expected);
	EXPECT_EQ(constantValue.toStringDec(), expected);
	expectRepresentation(
		value,
		{123456789012345678ULL, 0, STORAGE_MAX},
		true
	);
}
