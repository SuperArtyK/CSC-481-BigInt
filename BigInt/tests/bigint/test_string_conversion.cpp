//
// Created by artyk on 11/9/26.
//

#include <gtest/gtest.h>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
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


namespace {

struct SciCase {
	std::string input;
	std::size_t precision;
	bool pad;
	std::string expected;
};

void expectScientificCases(const std::initializer_list<SciCase>& cases) {
	for (const SciCase& testCase : cases) {
		SCOPED_TRACE(
			::testing::Message()
				<< "Input: " << testCase.input
				<< ", precision: " << testCase.precision
				<< ", pad: " << testCase.pad
		);

		BigInt value{std::string_view(testCase.input)};
		const std::vector<limb_t> originalLimbs = tester::getLimbs(value);
		const bool originalNegative = tester::getNegative(value);
		const BigInt& constantValue = value;

		EXPECT_EQ(constantValue.toStringSci(testCase.precision, testCase.pad), testCase.expected);
		expectRepresentation(value, originalLimbs, originalNegative);
	}
}


struct DisplayCase {
	std::string input;
	std::string expected;
};

void expectDefaultDisplayCases(const std::initializer_list<DisplayCase>& cases) {
	for (const DisplayCase& testCase : cases) {
		SCOPED_TRACE(::testing::Message() << "Input: " << testCase.input);

		BigInt value{std::string_view(testCase.input)};
		const std::vector<limb_t> originalLimbs = tester::getLimbs(value);
		const bool originalNegative = tester::getNegative(value);
		const BigInt& constantValue = value;

		EXPECT_EQ(constantValue.toString(), testCase.expected);
		expectRepresentation(value, originalLimbs, originalNegative);
	}
}

} // namespace


TEST(BigintStringConversion, ToStringSciZeroAndSmallValues) {
	expectScientificCases({
		{"0", 1, false, "0e+0"},
		{"0", 1, true, "0e+0"},
		{"0", 3, false, "0.00e+0"},
		{"0", 3, true, "0.00e+0"},
		{"0", 20, false, "0." + std::string(19, '0') + "e+0"},
		{"0", 20, true, "0." + std::string(19, '0') + "e+0"},
		{"7", 1, false, "7e+0"},
		{"7", 1, true, "7e+0"},
		{"7", 4, false, "7e+0"},
		{"7", 4, true, "7.000e+0"},
		{"-7", 4, false, "-7e+0"},
		{"-7", 4, true, "-7.000e+0"},
		{"42", 2, false, "4.2e+1"},
		{"42", 2, true, "4.2e+1"},
		{"42", 5, false, "4.2e+1"},
		{"42", 5, true, "4.2000e+1"},
	});
}

TEST(BigintStringConversion, ToStringSciPrecisionAndTrailingZeroes) {
	expectScientificCases({
		{"123000", 20, false, "1.23000e+5"},
		{"123000", 6, false, "1.23000e+5"},
		{"123000", 5, false, "1.23e+5"},
		{"123000", 3, false, "1.23e+5"},
		{"123000", 3, true, "1.23e+5"},
		{"123000", 4, true, "1.230e+5"},
		{"123000", 6, true, "1.23000e+5"},
		{"123000", 7, true, "1.230000e+5"},
		{"20000", 10, false, "2.0000e+4"},
		{"20000", 10, true, "2.000000000e+4"},
		{"20000", 3, false, "2e+4"},
		{"20000", 3, true, "2.00e+4"},
		{"10000", 1, false, "1e+4"},
		{"10000", 1, true, "1e+4"},
		{"-123000", 20, false, "-1.23000e+5"},
		{"-123000", 7, true, "-1.230000e+5"},
	});
}

TEST(BigintStringConversion, ToStringSciRoundsToNearestTiesAwayFromZero) {
	expectScientificCases({
		{"123440", 4, false, "1.234e+5"},
		{"123449", 4, false, "1.234e+5"},
		{"123450", 4, false, "1.235e+5"},
		{"123451", 4, false, "1.235e+5"},
		{"123499", 4, false, "1.235e+5"},
		{"123550", 4, false, "1.236e+5"},
		{"123450", 4, true, "1.235e+5"},
		{"149", 1, false, "1e+2"},
		{"150", 1, false, "2e+2"},
		{"159", 1, true, "2e+2"},
		{"-149", 1, false, "-1e+2"},
		{"-150", 1, false, "-2e+2"},
		{"120049", 3, false, "1.2e+5"},
		{"120049", 3, true, "1.20e+5"},
		{"100049", 3, false, "1e+5"},
		{"100049", 3, true, "1.00e+5"},
		{"123400", 4, false, "1.234e+5"},
		{"123400", 4, true, "1.234e+5"},
		{"123456789", 4, false, "1.235e+8"},
	});
}

TEST(BigintStringConversion, ToStringSciCarryPropagationAndExponent) {
	expectScientificCases({
		{"129950", 4, false, "1.3e+5"},
		{"129950", 4, true, "1.300e+5"},
		{"199950", 4, false, "2e+5"},
		{"999949", 4, false, "9.999e+5"},
		{"999950", 4, false, "1e+6"},
		{"999950", 4, true, "1.000e+6"},
		{"-999950", 4, false, "-1e+6"},
		{"-999950", 4, true, "-1.000e+6"},
		{"999950", 1, false, "1e+6"},
		{"999950", 1, true, "1e+6"},
		{std::string(20, '9'), 20, false, "9." + std::string(19, '9') + "e+19"},
		{std::string(20, '9'), 19, false, "1e+20"},
		{std::string(20, '9'), 19, true, "1." + std::string(18, '0') + "e+20"},
		{std::string(20, '9'), 1, false, "1e+20"},
		{"-" + std::string(20, '9'), 19, true, "-1." + std::string(18, '0') + "e+20"},
	});
}

TEST(BigintStringConversion, ToStringSciAcrossLimbBoundaries) {
	expectScientificCases({
		{"1000000000000000001", 19, false, "1." + std::string(17, '0') + "1e+18"},
		{"1000000000000000001", 18, false, "1e+18"},
		{"1000000000000000001", 18, true, "1." + std::string(17, '0') + "e+18"},
		{"123456789012345678901234567890", 20, false, "1.234567890123456789e+29"},
		{"123456789012345678901234567890", 20, true, "1.2345678901234567890e+29"},
		{"123456789012345678901234567890", 18, false, "1.23456789012345679e+29"},
		{"123456789012345678901234567890", 18, true, "1.23456789012345679e+29"},
		{"100000000000000000000000000001", 20, false, "1e+29"},
		{"100000000000000000000000000001", 30, false, "1." + std::string(28, '0') + "1e+29"},
		{"100000000000000000000000000001", 30, true, "1." + std::string(28, '0') + "1e+29"},
		{"-100000000000000000000000000001", 30, false, "-1." + std::string(28, '0') + "1e+29"},
	});
}

TEST(BigintStringConversion, ToStringSciPreservesInternalZeroLimbs) {
	BigInt value;
	tester::load(value, {1, 0, 1}, false);
	const BigInt& constantValue = value;

	EXPECT_EQ(constantValue.toStringSci(20, false), "1e+36");
	EXPECT_EQ(constantValue.toStringSci(20, true), "1." + std::string(19, '0') + "e+36");
	EXPECT_EQ(constantValue.toStringSci(37, false), "1." + std::string(35, '0') + "1e+36");
	expectRepresentation(value, {1, 0, 1}, false);
}

TEST(BigintStringConversion, ToStringSciLargeValueAndExponent) {
	const std::string decimal(5000, '9');
	BigInt value{std::string_view(decimal)};
	const BigInt& constantValue = value;
	const std::vector<limb_t> originalLimbs = tester::getLimbs(value);

	EXPECT_EQ(constantValue.toStringSci(20, false), "1e+5000");
	EXPECT_EQ(constantValue.toStringSci(20, true), "1." + std::string(19, '0') + "e+5000");
	EXPECT_EQ(constantValue.toStringSci(5000, false), "9." + std::string(4999, '9') + "e+4999");
	expectRepresentation(value, originalLimbs, false);
}

TEST(BigintStringConversion, ToStringSciUsesDefaultPrecisionAndCompactMode) {
	const BigInt value{"123456789012345678901"};
	EXPECT_EQ(value.toStringSci(), "1.234567890123456789e+20");
	EXPECT_EQ(value.toStringSci(), value.toStringSci(20, false));

	const BigInt zero;
	EXPECT_EQ(zero.toStringSci(), "0." + std::string(19, '0') + "e+0");
}

TEST(BigintStringConversion, ToStringSciRejectsZeroPrecision) {
	for (const std::string_view input : {std::string_view("0"), std::string_view("12345"), std::string_view("-98765")}) {
		SCOPED_TRACE(::testing::Message() << "Input: " << input);
		BigInt value{input};
		const std::vector<limb_t> originalLimbs = tester::getLimbs(value);
		const bool originalNegative = tester::getNegative(value);

		EXPECT_THROW(value.toStringSci(0, false), std::invalid_argument);
		EXPECT_THROW(value.toStringSci(0, true), std::invalid_argument);
		expectRepresentation(value, originalLimbs, originalNegative);
	}
}

TEST(BigintStringConversion, ToStringSciRejectsNonNormalisedRepresentation) {
	struct Case {
		std::initializer_list<limb_t> limbs;
		bool negative;
	};
	const Case cases[]{
		{{}, false},
		{{}, true},
		{{0}, true},
		{{0, 0}, false},
		{{0, 0}, true},
		{{5, 0}, false},
		{{10, 0, 0}, true},
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
		ASSERT_FALSE(value.isNormalised());

		EXPECT_THROW(value.toStringSci(4, false), std::logic_error);
		EXPECT_THROW(value.toStringSci(4, true), std::logic_error);
		expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
	}
}


TEST(BigintStringConversion, ToStringUsesExactDecimalForAtMostTwentyDigits) {
	expectDefaultDisplayCases({
		{"0", "0"},
		{"1", "1"},
		{"-1", "-1"},
		{"9", "9"},
		{"-9", "-9"},
		{"10", "10"},
		{"-42", "-42"},
		{"123456789012345678", "123456789012345678"},
		{"-999999999999999999", "-999999999999999999"},
		{"1000000000000000000", "1000000000000000000"},
		{"-1000000000000000000", "-1000000000000000000"},
		{"10000000000000000000", "10000000000000000000"},
		{"99999999999999999999", "99999999999999999999"},
		{"-99999999999999999999", "-99999999999999999999"},
		{"12345678901234567890", "12345678901234567890"},
		{"-12345678901234567890", "-12345678901234567890"},
		{"10000000000000000001", "10000000000000000001"},
	});
}

TEST(BigintStringConversion, ToStringUsesCompactScientificForMoreThanTwentyDigits) {
	expectDefaultDisplayCases({
		{"100000000000000000000", "1e+20"},
		{"-100000000000000000000", "-1e+20"},
		{"100000000000000000001", "1e+20"},
		{"200000000000000000000", "2e+20"},
		{"123456789012345678901", "1.234567890123456789e+20"},
		{"-123456789012345678901", "-1.234567890123456789e+20"},
		{"123456789012345678904", "1.234567890123456789e+20"},
		{"123456789012345678905", "1.2345678901234567891e+20"},
		{"123456789012345678995", "1.23456789012345679e+20"},
		{"123000000000000000000000000000", "1.23e+29"},
		{"-123000000000000000000000000000", "-1.23e+29"},
		{"987654321098765432123456789012345678", "9.8765432109876543212e+35"},
	});
}

TEST(BigintStringConversion, ToStringRoundsAcrossTheTwentyDigitBoundary) {
	expectDefaultDisplayCases({
		{std::string(20, '9'), std::string(20, '9')},
		{std::string(20, '9') + "0", "9." + std::string(19, '9') + "e+20"},
		{std::string(20, '9') + "4", "9." + std::string(19, '9') + "e+20"},
		{std::string(20, '9') + "5", "1e+21"},
		{"-" + std::string(20, '9') + "5", "-1e+21"},
	});
}

TEST(BigintStringConversion, ToStringHandlesLargeValuesAndInternalZeroLimbs) {
	const std::string allNines(5000, '9');
	const std::string powerOfTen = "1" + std::string(4999, '0');
	expectDefaultDisplayCases({
		{allNines, "1e+5000"},
		{"-" + allNines, "-1e+5000"},
		{powerOfTen, "1e+4999"},
	});

	BigInt value;
	tester::load(value, {1, 0, 1}, true);
	const BigInt& constantValue = value;
	EXPECT_EQ(constantValue.toString(), "-1e+36");
	expectRepresentation(value, {1, 0, 1}, true);
}

TEST(BigintStringConversion, ToStringRepeatedFormattingIsStable) {
	BigInt value("123456789012345678901");
	const std::vector<limb_t> originalLimbs = tester::getLimbs(value);
	const bool originalNegative = tester::getNegative(value);
	const BigInt& constantValue = value;

	EXPECT_EQ(constantValue.toString(), "1.234567890123456789e+20");
	EXPECT_EQ(constantValue.toString(), "1.234567890123456789e+20");
	expectRepresentation(value, originalLimbs, originalNegative);
}

TEST(BigintStringConversion, ToStringRejectsNonNormalisedRepresentation) {
	struct Case {
		std::initializer_list<limb_t> limbs;
		bool negative;
	};
	const Case cases[]{
		{{}, false},
		{{}, true},
		{{0}, true},
		{{0, 0}, false},
		{{0, 0}, true},
		{{5, 0}, false},
		{{10, 0, 0}, true},
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
		ASSERT_FALSE(value.isNormalised());

		EXPECT_THROW(value.toString(), std::logic_error);
		expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
	}
}
