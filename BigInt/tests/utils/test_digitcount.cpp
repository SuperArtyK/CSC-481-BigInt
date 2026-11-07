//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/utils.hpp"

#include <cstddef>
#include <limits>
#include <random>

// Independent reference implementation.
// Division toward zero works for both positive and negative values,
// including the minimum signed integer.
template <typename T>
constexpr std::size_t referenceDigitCount(T value) {
	if constexpr (std::is_same_v<T, bool>) {
		return 1;
	}
	else {
		std::size_t digits = 1;

		while (( value /= 10 ) != 0) {
			++digits;
		}

		return digits;
	}
}

// -------------------------------------------------------
// Basic and known-value tests
// -------------------------------------------------------

TEST(DigitCountTest, KnownValues) {
	EXPECT_EQ(digitCount(0), 1u);
	EXPECT_EQ(digitCount(1), 1u);
	EXPECT_EQ(digitCount(9), 1u);
	EXPECT_EQ(digitCount(10), 2u);
	EXPECT_EQ(digitCount(99), 2u);
	EXPECT_EQ(digitCount(100), 3u);
	EXPECT_EQ(digitCount(1000), 4u);

	EXPECT_EQ(digitCount(-1), 1u);
	EXPECT_EQ(digitCount(-9), 1u);
	EXPECT_EQ(digitCount(-10), 2u);
	EXPECT_EQ(digitCount(-99), 2u);
	EXPECT_EQ(digitCount(-100), 3u);

	EXPECT_EQ(digitCount(false), 1u);
	EXPECT_EQ(digitCount(true), 1u);
}

// -------------------------------------------------------
// Compile-time verification
// -------------------------------------------------------

static_assert(digitCount(0) == 1);
static_assert(digitCount(999) == 3);
static_assert(digitCount(-1000) == 4);

static_assert(
	digitCount(std::numeric_limits<std::int64_t>::lowest())
	== 19
);

static_assert(
	digitCount(std::numeric_limits<std::uint64_t>::max())
	== 20
);

// -------------------------------------------------------
// Typed tests: run the same tests for every integer type
// -------------------------------------------------------

template <typename T>
class DigitCountTypedTest : public ::testing::Test {
};

using IntegerTypes = ::testing::Types<
	char,
	signed char, unsigned char,
	short, unsigned short,
	int, unsigned int,
	long, unsigned long,
	long long, unsigned long long>;

TYPED_TEST_SUITE(DigitCountTypedTest, IntegerTypes);

// Test the limits of each type.
TYPED_TEST(DigitCountTypedTest, TypeLimits) {
	using T = TypeParam;
	using Limits = std::numeric_limits<T>;

	EXPECT_EQ(
		digitCount(Limits::lowest()),
		referenceDigitCount(Limits::lowest())
	);

	EXPECT_EQ(
		digitCount(Limits::max()),
		referenceDigitCount(Limits::max())
	);

	EXPECT_EQ(digitCount(T{0}), 1u);
}

// Test every representable power-of-ten boundary,
// together with the immediately adjacent values.
TYPED_TEST(DigitCountTypedTest, DecimalBoundaries) {
	using T = TypeParam;
	using Limits = std::numeric_limits<T>;

	const auto check = [](T value) {
		EXPECT_EQ(digitCount(value), referenceDigitCount(value))
            << "Input: " << ::testing::PrintToString(value);
	};

	T power = 1;

	while (true) {
		check(power);
		check(static_cast<T>(power - 1));

		if (power < Limits::max()) {
			check(static_cast<T>(power + 1));
		}

		if constexpr (Limits::is_signed) {
			const T negative = static_cast<T>(-power);

			check(negative);

			if (negative > Limits::lowest()) {
				check(static_cast<T>(negative - 1));
			}

			check(static_cast<T>(negative + 1));
		}

		// Prevent overflow when generating the next power.
		if (power > Limits::max() / 10) {
			break;
		}

		power = static_cast<T>(power * 10);
	}
}

// -------------------------------------------------------
// Deterministic random sampling
// -------------------------------------------------------

TYPED_TEST(DigitCountTypedTest, RandomSamples) {
	using T = TypeParam;

	std::mt19937_64 rng(0x481);

	constexpr int sampleCount = 1000;

	for (int i = 0; i < sampleCount; ++i) {
		const T value = static_cast<T>(rng());

		ASSERT_EQ(digitCount(value), referenceDigitCount(value))
		<< "Sample: " << i
		<< ", input: " << ::testing::PrintToString(value);
	}
}
