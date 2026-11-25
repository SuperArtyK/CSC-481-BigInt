//
// Tests for BigInt unary plus, unary minus, and in-place negation.
//

#include <gtest/gtest.h>

#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

static_assert(std::is_same_v<decltype(+std::declval<const BigInt&>()), BigInt>);
static_assert(std::is_same_v<decltype(-std::declval<const BigInt&>()), BigInt>);
static_assert(std::is_same_v<decltype(std::declval<BigInt&>().negate()), BigInt&>);

TEST(BigintUnaryOperators, PlusReturnsAnIndependentUnchangedValue) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{0}, false},
        {{1}, false},
        {{1}, true},
        {{STORAGE_MAX}, false},
        {{STORAGE_MAX}, true},
        {{0, 1}, false},
        {{42, 0, 987654321098765432ULL}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(
            ::testing::Message()
                << "Limbs: " << ::testing::PrintToString(std::vector<limb_t>(testCase.limbs))
                << ", negative: " << testCase.negative
        );

        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());
        const BigInt& constantValue = value;

        BigInt result = +constantValue;
        expectRepresentation(result, std::vector<limb_t>(testCase.limbs), testCase.negative);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
        EXPECT_TRUE(result.isNormalised());
    }
}

TEST(BigintUnaryOperators, PlusDoesNotAliasSourceStorage) {
    BigInt value;
    tester::load(value, {12, 0, 987654321098765432ULL}, true);
    const BigInt& constantValue = value;

    BigInt result = +constantValue;
    ASSERT_TRUE(result.setLimbRaw(0, 99));
    result.setNegative(false);

    expectRepresentation(value, {12, 0, 987654321098765432ULL}, true);
    expectRepresentation(result, {99, 0, 987654321098765432ULL}, false);
}

TEST(BigintUnaryOperators, MinusFlipsOnlyTheSignOfNonzeroValues) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{1}, false},
        {{1}, true},
        {{42}, false},
        {{42}, true},
        {{STORAGE_MAX}, false},
        {{STORAGE_MAX}, true},
        {{0, 1}, false},
        {{0, 1}, true},
        {{123456789012345678ULL, 0, 987654321098765432ULL}, false},
        {{123456789012345678ULL, 0, 987654321098765432ULL}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(
            ::testing::Message()
                << "Limbs: " << ::testing::PrintToString(std::vector<limb_t>(testCase.limbs))
                << ", negative: " << testCase.negative
        );

        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());
        const BigInt& constantValue = value;

        BigInt result = -constantValue;
        expectRepresentation(result, std::vector<limb_t>(testCase.limbs), !testCase.negative);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
        EXPECT_TRUE(result.isNormalised());
    }
}

TEST(BigintUnaryOperators, UnaryOperationsPreserveCanonicalZero) {
    const BigInt values[]{BigInt(0), BigInt("-0"), BigInt("-0.999")};

    for (const BigInt& value : values) {
        ASSERT_TRUE(value.isNormalised());

        const BigInt positive = +value;
        const BigInt negative = -value;
        const BigInt twiceNegated = -(-value);

        expectRepresentation(value, {0}, false);
        expectRepresentation(positive, {0}, false);
        expectRepresentation(negative, {0}, false);
        expectRepresentation(twiceNegated, {0}, false);
    }
}

TEST(BigintUnaryOperators, DoubleNegationRestoresOriginalValue) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{0}, false},
        {{7}, false},
        {{7}, true},
        {{STORAGE_MAX, STORAGE_MAX}, false},
        {{STORAGE_MAX, STORAGE_MAX}, true},
        {{1, 0, 0, 1}, true},
    };

    for (const Case& testCase : cases) {
        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());

        BigInt result = -(-value);
        expectRepresentation(result, std::vector<limb_t>(testCase.limbs), testCase.negative);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
    }
}

TEST(BigintUnaryOperators, UnaryOperatorsWorkOnConstValuesAndTemporaries) {
    const BigInt value("-123456789012345678901234567890");

    EXPECT_EQ((+value).toStringDec(), "-123456789012345678901234567890");
    EXPECT_EQ((-value).toStringDec(), "123456789012345678901234567890");
    EXPECT_EQ((-BigInt(42)).toStringDec(), "-42");
    EXPECT_EQ((+BigInt(-42)).toStringDec(), "-42");
    EXPECT_EQ((-(-BigInt(42))).toStringDec(), "42");
    EXPECT_EQ(value.toStringDec(), "-123456789012345678901234567890");
}

TEST(BigintUnaryOperators, NegateModifiesInPlaceAndReturnsSelf) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{0}, false},
        {{1}, false},
        {{1}, true},
        {{STORAGE_MAX}, false},
        {{STORAGE_MAX}, true},
        {{0, 1}, false},
        {{0, 1}, true},
        {{1, 0, 1}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(
            ::testing::Message()
                << "Limbs: " << ::testing::PrintToString(std::vector<limb_t>(testCase.limbs))
                << ", negative: " << testCase.negative
        );

        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());

        BigInt& returned = value.negate();
        EXPECT_EQ(&returned, &value);
        const bool expectedNegative = value.isZero() ? false : !testCase.negative;
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), expectedNegative);
        EXPECT_TRUE(value.isNormalised());
    }
}

TEST(BigintUnaryOperators, NegateTwiceRestoresStoredRepresentation) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{0}, false},
        {{17}, false},
        {{17}, true},
        {{0, 1}, false},
        {{42, 0, 987654321098765432ULL}, true},
    };

    for (const Case& testCase : cases) {
        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());

        BigInt& returned = value.negate().negate();
        EXPECT_EQ(&returned, &value);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
        EXPECT_TRUE(value.isNormalised());
    }
}

TEST(BigintUnaryOperators, NegateAndUnaryMinusHaveDifferentMutationSemantics) {
    BigInt source("123456789012345678901");
    const std::vector<limb_t> originalLimbs = tester::getLimbs(source);

    const BigInt negatedCopy = -source;
    expectRepresentation(source, originalLimbs, false);
    expectRepresentation(negatedCopy, originalLimbs, true);

    BigInt& reference = source.negate();
    EXPECT_EQ(&reference, &source);
    expectRepresentation(source, originalLimbs, true);
    expectRepresentation(negatedCopy, originalLimbs, true);

    source.negate();
    expectRepresentation(source, originalLimbs, false);
    expectRepresentation(negatedCopy, originalLimbs, true);
}

TEST(BigintUnaryOperators, UnaryOperationsHandleVeryLargeMagnitudeExactly) {
    const std::string digits = "1" + std::string(4998, '0') + "7";
    const BigInt original{std::string_view{digits}};
    const BigInt positive = +original;
    const BigInt negative = -original;
    const BigInt recovered = -negative;

    EXPECT_EQ(original.toStringDec(), digits);
    EXPECT_EQ(positive.toStringDec(), digits);
    EXPECT_EQ(negative.toStringDec(), "-" + digits);
    EXPECT_EQ(recovered.toStringDec(), digits);

    const auto limbs = tester::getLimbs(original);
    expectRepresentation(positive, limbs, false);
    expectRepresentation(negative, limbs, true);
    expectRepresentation(recovered, limbs, false);
    expectRepresentation(original, limbs, false);
}
