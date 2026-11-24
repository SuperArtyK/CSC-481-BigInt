//
// Tests for BigInt insertion into standard output streams.
//

#include <gtest/gtest.h>
#include <iomanip>
#include <initializer_list>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintStreamOutput, ExactDecimalValues) {
    struct Case {
        std::string_view input;
        std::string_view expected;
    };

    const Case cases[]{
        {"0", "0"},
        {"-0", "0"},
        {"1", "1"},
        {"-1", "-1"},
        {"12345", "12345"},
        {"-12345", "-12345"},
        {"999999999999999999", "999999999999999999"},
        {"-1000000000000000000", "-1000000000000000000"},
        {"99999999999999999999", "99999999999999999999"},
        {"-99999999999999999999", "-99999999999999999999"},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.input));
        const BigInt value{testCase.input};
        std::ostringstream stream;

        stream << value;

        EXPECT_EQ(stream.str(), testCase.expected);
        EXPECT_TRUE(stream.good());
    }
}

TEST(BigintStreamOutput, ScientificDisplayBeyondTwentyDigits) {
    struct Case {
        std::string_view input;
        std::string_view expected;
    };

    const Case cases[]{
        {"100000000000000000000", "1e+20"},
        {"-100000000000000000000", "-1e+20"},
        {"123456789012345678901", "1.234567890123456789e+20"},
        {"-123456789012345678901", "-1.234567890123456789e+20"},
        {"123456789012345678905", "1.2345678901234567891e+20"},
        {"999999999999999999995", "1e+21"},
        {"-999999999999999999995", "-1e+21"},
        {"123000000000000000000000000000", "1.23e+29"},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.input));
        const BigInt value{testCase.input};
        std::ostringstream stream;

        stream << value;

        EXPECT_EQ(stream.str(), testCase.expected);
        EXPECT_TRUE(stream.good());
    }
}

TEST(BigintStreamOutput, InsertionReturnsStreamAndSupportsChaining) {
    const BigInt small(-42);
    const BigInt large("123456789012345678901");
    std::ostringstream stream;

    std::ostream& returned = (stream << "a=" << small << ", b=" << large << '!');

    EXPECT_EQ(&returned, static_cast<std::ostream*>(&stream));
    EXPECT_EQ(stream.str(), "a=-42, b=1.234567890123456789e+20!");
    EXPECT_TRUE(stream.good());
}

TEST(BigintStreamOutput, NumericManipulatorsDoNotChangeBigIntFormatting) {
    const BigInt small("12345");
    const BigInt large("123456789012345678901");
    std::ostringstream stream;

    stream << std::hex << std::showbase << std::showpos << std::uppercase
           << std::scientific << std::setprecision(3)
           << small << '|' << large;

    EXPECT_EQ(stream.str(), "12345|1.234567890123456789e+20");
    EXPECT_TRUE(stream.good());
}

TEST(BigintStreamOutput, RepeatedInsertionMatchesDefaultString) {
    const BigInt values[]{
        BigInt(0),
        BigInt(-123),
        BigInt("100000000000000000000"),
        BigInt("-123456789012345678901"),
    };

    for (const BigInt& value : values) {
        std::ostringstream stream;
        stream << value << '|' << value;

        const std::string expected = value.toString();
        EXPECT_EQ(stream.str(), expected + "|" + expected);
        EXPECT_TRUE(stream.good());
    }
}

TEST(BigintStreamOutput, HandlesVeryLargeMagnitudes) {
    const std::string digits(5000, '9');
    const BigInt positive{std::string_view{digits}};
    const std::string negativeDigits = "-" + digits;
    const BigInt negative{std::string_view{negativeDigits}};
    std::ostringstream stream;

    stream << positive << ' ' << negative;

    EXPECT_EQ(stream.str(), "1e+5000 -1e+5000");
    EXPECT_TRUE(stream.good());
}

TEST(BigintStreamOutput, PreservesStoredRepresentation) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
        std::string_view expected;
    };

    const Case cases[]{
        {{0}, false, "0"},
        {{42}, false, "42"},
        {{42}, true, "-42"},
        {{0, 1}, false, "1000000000000000000"},
        {{1, 0, 1}, true, "-1e+36"},
        {{STORAGE_MAX, STORAGE_MAX}, false, "1e+36"},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(std::vector<limb_t>(testCase.limbs)));
        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_TRUE(value.isNormalised());
        const BigInt& constantValue = value;
        std::ostringstream stream;

        stream << constantValue;
        EXPECT_EQ(stream.str(), testCase.expected);
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
    }
}

TEST(BigintStreamOutput, DoesNotWriteToFailedStream) {
    std::ostringstream stream;
    stream.setstate(std::ios::failbit);

    stream << BigInt(42);

    EXPECT_TRUE(stream.fail());
    EXPECT_EQ(stream.str(), "");
}

TEST(BigintStreamOutput, RejectsNonNormalisedRepresentationWithoutMutation) {
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
        {{42, 0, 0}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(std::vector<limb_t>(testCase.limbs)));
        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_FALSE(value.isNormalised());
        const BigInt& constantValue = value;
        std::ostringstream stream;
        stream << "prefix";

        EXPECT_THROW(stream << constantValue, std::logic_error);
        EXPECT_EQ(stream.str(), "prefix");
        EXPECT_TRUE(stream.good());
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
    }
}
