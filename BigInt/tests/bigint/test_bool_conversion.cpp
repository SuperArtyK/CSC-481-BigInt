//
// Tests for BigInt's explicit Boolean conversion.
//

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

static_assert(std::is_constructible_v<bool, const BigInt&>);
static_assert(!std::is_convertible_v<const BigInt&, bool>);
static_assert(!std::is_convertible_v<const BigInt&, int>);
static_assert(noexcept(static_cast<bool>(std::declval<const BigInt&>())));

TEST(BigintBoolConversion, ZeroIsFalse) {
    const BigInt zero;
    const BigInt parsedZero("-0");
    const BigInt truncatedZero("-0.999");

    EXPECT_FALSE(static_cast<bool>(zero));
    EXPECT_FALSE(static_cast<bool>(parsedZero));
    EXPECT_FALSE(static_cast<bool>(truncatedZero));
    EXPECT_TRUE(zero.isNormalised());
    EXPECT_TRUE(parsedZero.isNormalised());
    EXPECT_TRUE(truncatedZero.isNormalised());
}

TEST(BigintBoolConversion, NonzeroValuesAreTrue) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
        bool expected;
    };

    const Case cases[]{
        {{0}, false, false},
        {{1}, false, true},
        {{1}, true, true},
        {{42}, false, true},
        {{42}, true, true},
        {{STORAGE_MAX}, false, true},
        {{STORAGE_MAX}, true, true},
        {{0, 1}, false, true},
        {{0, 1}, true, true},
        {{0, 0, 1}, false, true},
        {{0, 0, 1}, true, true},
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
        ASSERT_TRUE(value.isNormalised());
        const BigInt& constantValue = value;

        EXPECT_EQ(static_cast<bool>(constantValue), testCase.expected);
        EXPECT_EQ(static_cast<bool>(constantValue), testCase.expected);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
    }
}

TEST(BigintBoolConversion, WorksInConditionalContexts) {
    const BigInt zero(0);
    const BigInt positive(7);
    const BigInt negative(-7);

    bool enteredZero = false;
    bool enteredPositive = false;
    bool enteredNegative = false;

    if (zero) {
        enteredZero = true;
    }
    if (positive) {
        enteredPositive = true;
    }
    if (negative) {
        enteredNegative = true;
    }

    EXPECT_FALSE(enteredZero);
    EXPECT_TRUE(enteredPositive);
    EXPECT_TRUE(enteredNegative);

    EXPECT_TRUE(!zero);
    EXPECT_FALSE(!positive);
    EXPECT_FALSE(!negative);

    EXPECT_FALSE(zero && true);
    EXPECT_TRUE(positive && true);
    EXPECT_TRUE(negative && true);
    EXPECT_TRUE(zero || true);
    EXPECT_FALSE(zero || false);
    EXPECT_TRUE(positive || false);
    EXPECT_TRUE(negative || false);

    EXPECT_EQ(zero ? 1 : 0, 0);
    EXPECT_EQ(positive ? 1 : 0, 1);
    EXPECT_EQ(negative ? 1 : 0, 1);
}

TEST(BigintBoolConversion, ConversionDoesNotModifyRepresentation) {
    BigInt value;
    tester::load(value, {123456789012345678ULL, 0, 987654321098765432ULL}, true);
    const BigInt& constantValue = value;

    EXPECT_TRUE(static_cast<bool>(constantValue));
    EXPECT_FALSE(!constantValue);
    EXPECT_TRUE(static_cast<bool>(constantValue));
    expectRepresentation(value, {123456789012345678ULL, 0, 987654321098765432ULL}, true);

    tester::load(value, {0}, false);
    EXPECT_FALSE(static_cast<bool>(constantValue));
    EXPECT_TRUE(!constantValue);
    expectRepresentation(value, {0}, false);
}
