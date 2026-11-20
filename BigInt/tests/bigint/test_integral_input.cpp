#include <gtest/gtest.h>

#include <charconv>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using bigint_test::expectRepresentation;
using tester = bigint_test::Inspector;

namespace {

template <std::integral T>
std::string builtinIntegerText(const T input) {
    if constexpr (std::same_as<T, bool>) {
        return input ? "1" : "0";
    } else {
        char buffer[64];
        const auto [end, ec] = std::to_chars(buffer, buffer + sizeof(buffer), input);
        if (ec != std::errc{}) {
            return "unexpected to_chars error";
        }
        return std::string(buffer, end);
    }
}

template <typename T>
class BigintIntegralInputTyped : public ::testing::Test {};

using IntegralTypes = ::testing::Types<
    bool, char, signed char, unsigned char,
    short, unsigned short, int, unsigned int,
    long, unsigned long, long long, unsigned long long,
    wchar_t, char8_t, char16_t, char32_t
>;
TYPED_TEST_SUITE(BigintIntegralInputTyped, IntegralTypes);

} // namespace

TYPED_TEST(BigintIntegralInputTyped, ConstructionAndAssignmentPreserveExactValue) {
    using T = TypeParam;
    constexpr T cases[]{
        T{0}, T{1}, std::numeric_limits<T>::lowest(),
        std::numeric_limits<T>::max()
    };

    for (const T input : cases) {
        SCOPED_TRACE(builtinIntegerText(input));
        const std::string expected = builtinIntegerText(input);

        const BigInt constructed{input};
        EXPECT_EQ(constructed.toStringDec(), expected);
        EXPECT_TRUE(constructed.isNormalised());

        BigInt assigned;
        tester::load(assigned, {3, 0, 0}, true);
        BigInt& returned = (assigned = input);
        EXPECT_EQ(&returned, &assigned);
        EXPECT_EQ(assigned.toStringDec(), expected);
        EXPECT_TRUE(assigned.isNormalised());
    }
}

TEST(BigintIntegralInput, ChecksLimbBoundariesAndSignedExtremes) {
    const BigInt limbMaximum{static_cast<std::uint64_t>(STORAGE_MAX)};
    expectRepresentation(limbMaximum, {STORAGE_MAX}, false);

    const BigInt limbBase{static_cast<std::uint64_t>(STORAGE_MASK)};
    expectRepresentation(limbBase, {0, 1}, false);

    const BigInt unsignedMaximum{std::numeric_limits<std::uint64_t>::max()};
    expectRepresentation(unsignedMaximum, {446744073709551615ULL, 18}, false);

    const BigInt signedMinimum{std::numeric_limits<std::int64_t>::min()};
    expectRepresentation(signedMinimum, {223372036854775808ULL, 9}, true);

    const BigInt signedMaximum{std::numeric_limits<std::int64_t>::max()};
    expectRepresentation(signedMaximum, {223372036854775807ULL, 9}, false);
}

TEST(BigintIntegralInput, AssigningZeroRestoresCanonicalRepresentation) {
    BigInt value;
    tester::load(value, {0, 0, 0}, true);
    value = 0;
    expectRepresentation(value, {0}, false);

    tester::load(value, {}, true);
    value = false;
    expectRepresentation(value, {0}, false);
}
