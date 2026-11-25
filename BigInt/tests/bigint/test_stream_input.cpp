//
// Tests for BigInt extraction from standard input streams.
//

#include <gtest/gtest.h>

#include <array>
#include <initializer_list>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintStreamInput, ExtractsValidNumericTokens) {
    struct Case {
        std::string_view token;
        std::vector<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {"0", {0}, false},
        {"-0", {0}, false},
        {"+0", {0}, false},
        {"-000.999", {0}, false},
        {".5", {0}, false},
        {"-.5", {0}, false},
        {"+5.", {5}, false},
        {"-123.99", {123}, true},
        {"00042", {42}, false},
        {"123456789012345678", {123456789012345678ULL}, false},
        {"1000000000000000000", {0, 1}, false},
        {"-1234567890123456789", {234567890123456789ULL, 1}, true},
        {"999999999999999999999999999999999999", {STORAGE_MAX, STORAGE_MAX}, false},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.token));

        std::istringstream stream{std::string(testCase.token)};
        BigInt value(-777);

        std::istream& returned = (stream >> value);

        EXPECT_EQ(&returned, static_cast<std::istream*>(&stream));
        EXPECT_FALSE(stream.fail());
        EXPECT_TRUE(value.isNormalised());
        expectRepresentation(value, testCase.limbs, testCase.negative);
    }
}

TEST(BigintStreamInput, ExtractsSuccessiveWhitespaceDelimitedTokens) {
    std::istringstream stream(" \t\n  12\t-34\n+5.9  0006 ");
    BigInt first(-1);
    BigInt second(-1);
    BigInt third(-1);
    BigInt fourth(-1);

    std::istream& returned = (stream >> first >> second >> third >> fourth);

    EXPECT_EQ(&returned, static_cast<std::istream*>(&stream));
    EXPECT_FALSE(stream.fail());
    expectRepresentation(first, {12}, false);
    expectRepresentation(second, {34}, true);
    expectRepresentation(third, {5}, false);
    expectRepresentation(fourth, {6}, false);
}

TEST(BigintStreamInput, StopsAtTokenBoundaryWithoutConsumingNextValue) {
    std::istringstream stream("12 34");
    BigInt first(-1);
    std::string nextToken;

    stream >> first;
    EXPECT_FALSE(stream.fail());
    EXPECT_EQ(first.toStringDec(), "12");

    stream >> nextToken;
    EXPECT_FALSE(stream.fail());
    EXPECT_EQ(nextToken, "34");
}

TEST(BigintStreamInput, SuccessfulExtractionNormalisesDestination) {
    struct Case {
        std::initializer_list<limb_t> initialLimbs;
        bool initialNegative;
        std::string_view input;
        std::vector<limb_t> expectedLimbs;
        bool expectedNegative;
    };

    const Case cases[]{
        {{123, 0, 0}, true, "42", {42}, false},
        {{0}, true, "-0", {0}, false},
        {{}, true, "-1234567890123456789", {234567890123456789ULL, 1}, true},
        {{0, 0}, false, ".9", {0}, false},
        {{88, 0}, true, "-7.75", {7}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.input));

        BigInt value;
        tester::load(value, testCase.initialLimbs, testCase.initialNegative);
        ASSERT_FALSE(value.isNormalised());
        std::istringstream stream{std::string(testCase.input)};

        stream >> value;

        EXPECT_FALSE(stream.fail());
        EXPECT_TRUE(value.isNormalised());
        expectRepresentation(value, testCase.expectedLimbs, testCase.expectedNegative);
    }
}

TEST(BigintStreamInput, MalformedTokensSetFailbitAndPreserveDestination) {
    constexpr std::array<std::string_view, 15> invalidTokens{
        "+", "-", ".", "-.", "12x", "1.2.3", "1e20", "1E+2",
        "1,000", "0x10", "nan", "inf", "++1", "--1", "1_000"
    };

    for (const std::string_view token : invalidTokens) {
        SCOPED_TRACE(::testing::PrintToString(token));

        BigInt value;
        tester::load(value, {123, 0, 0}, true);
        std::istringstream stream{std::string(token)};

        std::istream& returned = (stream >> value);

        EXPECT_EQ(&returned, static_cast<std::istream*>(&stream));
        EXPECT_TRUE(stream.fail());
        EXPECT_FALSE(stream.bad());
        expectRepresentation(value, {123, 0, 0}, true);
    }
}

TEST(BigintStreamInput, FailedExtractionPreservesOtherNonNormalisedStates) {
    struct Case {
        std::initializer_list<limb_t> limbs;
        bool negative;
    };

    const Case cases[]{
        {{}, false},
        {{}, true},
        {{0}, true},
        {{0, 0}, false},
        {{42, 0, 0}, true},
    };

    for (const Case& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(std::vector<limb_t>(testCase.limbs)));

        BigInt value;
        tester::load(value, testCase.limbs, testCase.negative);
        ASSERT_FALSE(value.isNormalised());
        std::istringstream stream("invalid");

        stream >> value;

        EXPECT_TRUE(stream.fail());
        expectRepresentation(value, std::vector<limb_t>(testCase.limbs), testCase.negative);
    }
}

TEST(BigintStreamInput, EmptyOrWhitespaceOnlyStreamFailsWithoutAssigningZero) {
    constexpr std::array<std::string_view, 5> inputs{
        "", " ", "\t", " \t\r\n ", "\n\n"
    };

    for (const std::string_view input : inputs) {
        SCOPED_TRACE(::testing::PrintToString(input));

        BigInt value(-99);
        std::istringstream stream{std::string(input)};

        stream >> value;

        EXPECT_TRUE(stream.fail());
        EXPECT_TRUE(stream.eof());
        expectRepresentation(value, {99}, true);
    }
}

TEST(BigintStreamInput, FailedTokenIsConsumedAndExtractionCanResume) {
    std::istringstream stream("invalid 123 -5");
    BigInt value(77);

    stream >> value;
    EXPECT_TRUE(stream.fail());
    expectRepresentation(value, {77}, false);

    stream.clear();
    stream >> value;
    EXPECT_FALSE(stream.fail());
    expectRepresentation(value, {123}, false);

    stream >> value;
    EXPECT_FALSE(stream.fail());
    expectRepresentation(value, {5}, true);
}

TEST(BigintStreamInput, FailbitExceptionMaskThrowsAndPreservesDestination) {
    constexpr std::array<std::string_view, 3> invalidInputs{
        "bad", "", "  \t "
    };

    for (const std::string_view input : invalidInputs) {
        SCOPED_TRACE(::testing::PrintToString(input));

        BigInt value;
        tester::load(value, {42, 0, 0}, true);
        std::istringstream stream{std::string(input)};
        stream.exceptions(std::ios::failbit);

        EXPECT_THROW(stream >> value, std::ios_base::failure);
        EXPECT_TRUE(stream.fail());
        expectRepresentation(value, {42, 0, 0}, true);
    }
}

TEST(BigintStreamInput, PreexistingFailedStreamDoesNotConsumeInputOrModifyDestination) {
    std::istringstream stream("123");
    stream.setstate(std::ios::failbit);
    BigInt value;
    tester::load(value, {42, 0}, true);

    stream >> value;

    EXPECT_TRUE(stream.fail());
    expectRepresentation(value, {42, 0}, true);

    stream.clear();
    stream >> value;
    EXPECT_FALSE(stream.fail());
    expectRepresentation(value, {123}, false);
}

TEST(BigintStreamInput, LongDecimalTokenIsParsedWithoutPrecisionLoss) {
    const std::string digits = "1" + std::string(4999, '0');
    std::istringstream stream(digits);
    BigInt value(-1);

    stream >> value;

    ASSERT_FALSE(stream.fail());
    EXPECT_TRUE(value.isNormalised());
    EXPECT_EQ(value.getLimbCount(), 278u);
    EXPECT_EQ(value.toStringDec(), digits);
}

TEST(BigintStreamInput, StreamBaseFlagsDoNotChangeDecimalTokenInterpretation) {
    std::istringstream stream("10 0x10");
    stream >> std::hex;
    BigInt value(-42);

    stream >> value;
    EXPECT_FALSE(stream.fail());
    expectRepresentation(value, {10}, false);

    stream >> value;
    EXPECT_TRUE(stream.fail());
    expectRepresentation(value, {10}, false);
}

TEST(BigintStreamInput, NoSkipWsRejectsLeadingWhitespace) {
    std::istringstream stream(" 123");
    stream >> std::noskipws;
    BigInt value(42);

    stream >> value;

    EXPECT_TRUE(stream.fail());
    expectRepresentation(value, {42}, false);

    stream.clear();
    stream >> std::skipws >> value;
    EXPECT_FALSE(stream.fail());
    expectRepresentation(value, {123}, false);
}
