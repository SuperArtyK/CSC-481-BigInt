#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using bigint_test::expectRepresentation;
using tester = bigint_test::Inspector;

static_assert(std::is_constructible_v<BigInt, std::string_view>);
static_assert(!std::is_convertible_v<std::string_view, BigInt>);

namespace {

struct StringCase {
    std::string_view input;
    std::vector<limb_t> limbs;
    bool negative;
    std::string_view exactText;
};

} // namespace

TEST(BigintStringInput, ConstructionProducesExactCanonicalRepresentation) {
    const std::array cases{
        StringCase{"", {0}, false, "0"},
        StringCase{" \t ", {0}, false, "0"},
        StringCase{"0", {0}, false, "0"},
        StringCase{"-0", {0}, false, "0"},
        StringCase{"-000.999", {0}, false, "0"},
        StringCase{"-.5", {0}, false, "0"},
        StringCase{".5", {0}, false, "0"},
        StringCase{"+5.", {5}, false, "5"},
        StringCase{" -00123.99\t", {123}, true, "-123"},
        StringCase{"123456789012345678", {123456789012345678ULL}, false,
                   "123456789012345678"},
        StringCase{"1234567890123456789", {234567890123456789ULL, 1}, false,
                   "1234567890123456789"},
        StringCase{"1000000000000000000", {0, 1}, false,
                   "1000000000000000000"},
        StringCase{"1000000000000000001", {1, 1}, false,
                   "1000000000000000001"},
        StringCase{"999999999999999999999999999999999999",
                   {STORAGE_MAX, STORAGE_MAX}, false,
                   "999999999999999999999999999999999999"},
        StringCase{"0001000000000000000000", {0, 1}, false,
                   "1000000000000000000"},
    };

    for (const auto& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.input));
        const BigInt value{testCase.input};
        expectRepresentation(value, testCase.limbs, testCase.negative);
        EXPECT_TRUE(value.isNormalised());
        EXPECT_EQ(value.toStringDec(), testCase.exactText);
    }
}

TEST(BigintStringInput, AssignmentReplacesPreviousRepresentation) {
    BigInt value;
    tester::load(value, {11, 22, 0, 0}, true);

    BigInt& returned = (value = std::string_view{"1234567890123456789"});
    EXPECT_EQ(&returned, &value);
    expectRepresentation(value, {234567890123456789ULL, 1}, false);
    EXPECT_TRUE(value.isNormalised());

    value = "-42.75";
    expectRepresentation(value, {42}, true);

    value = "\t \t";
    expectRepresentation(value, {0}, false);
    EXPECT_TRUE(value.isNormalised());

    tester::load(value, {}, true);
    value = ".5";
    expectRepresentation(value, {0}, false);

    tester::load(value, {0}, true);
    value = "123";
    expectRepresentation(value, {123}, false);
}

TEST(BigintStringInput, InvalidConstructionThrows) {
    constexpr std::array<std::string_view, 11> cases{
        "+", "-", ".", "-.", "1.2.3", "1e20", "1,000",
        "1 2", "\n1", "1\r", "12x"
    };

    for (const auto input : cases) {
        SCOPED_TRACE(::testing::PrintToString(input));
        EXPECT_THROW((BigInt{input}), std::invalid_argument);
    }
}

TEST(BigintStringInput, InvalidAssignmentPreservesNonNormalizedStorage) {
    constexpr std::array<std::string_view, 9> cases{
        "+", "-", ".", "1.2.3", "1e20", "1,000",
        "1 2", "\n1", "12x"
    };

    for (const auto input : cases) {
        SCOPED_TRACE(::testing::PrintToString(input));
        BigInt value;
        tester::load(value, {123, 0, 0}, true);
        EXPECT_THROW(value = input, std::invalid_argument);
        expectRepresentation(value, {123, 0, 0}, true);
    }
}

TEST(BigintStringInput, InvalidAssignmentPreservesEmptyStorage) {
    BigInt value;
    tester::load(value, {}, true);

    EXPECT_THROW(value = "1.2.3", std::invalid_argument);
    expectRepresentation(value, {}, true);
}

TEST(BigintStringInput, LongDecimalStringsRoundTripExactly) {
    std::string digits;
    digits.reserve(5000);
    for (std::size_t i = 0; i < 5000; ++i) {
        digits.push_back(static_cast<char>('0' + (i % 10)));
    }
    digits.front() = '1'; // Ensure the first digit is significant.

    const BigInt positive{std::string_view{digits}};
    ASSERT_TRUE(positive.isNormalised());
    EXPECT_EQ(positive.toStringDec(), digits);
    EXPECT_EQ(positive.getLimbCount(), 278u);

    BigInt negative;
    negative = "-" + digits + ".999";
    ASSERT_TRUE(negative.isNormalised());
    EXPECT_EQ(negative.toStringDec(), "-" + digits);
    EXPECT_EQ(negative.getLimbCount(), 278u);
}

TEST(BigintStringInput, LeadingZeroesDoNotAllocateRedundantLimbs) {
    const std::string input = std::string(5000, '0') + "123.75";
    const BigInt value{std::string_view{input}};

    expectRepresentation(value, {123}, false);
    EXPECT_TRUE(value.isNormalised());
}
