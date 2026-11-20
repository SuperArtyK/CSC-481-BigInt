#include <gtest/gtest.h>

#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using bigint_test::expectRepresentation;
using tester = bigint_test::Inspector;

namespace {

// Independent decimal oracle for 2^exponent; no floating-point formatting.
std::string decimalPowerOfTwo(const int exponent) {
    std::string digits = "1";
    for (int power = 0; power < exponent; ++power) {
        int carry = 0;
        for (std::size_t i = digits.size(); i-- > 0;) {
            const int doubled = 2 * (digits[i] - '0') + carry;
            digits[i] = static_cast<char>('0' + doubled % 10);
            carry = doubled / 10;
        }
        if (carry != 0) {
            digits.insert(digits.begin(), static_cast<char>('0' + carry));
        }
    }
    return digits;
}

template <typename T>
void expectFloatValue(const T input, const std::string_view expected) {
    const BigInt constructed{input};
    EXPECT_EQ(constructed.toStringDec(), expected);
    EXPECT_TRUE(constructed.isNormalised());

    BigInt assigned;
    tester::load(assigned, {42, 0, 0}, true);
    BigInt& returned = (assigned = input);
    EXPECT_EQ(&returned, &assigned);
    EXPECT_EQ(assigned.toStringDec(), expected);
    EXPECT_TRUE(assigned.isNormalised());
}

template <typename T>
class BigintFloatInputTyped : public ::testing::Test {};

using FloatingTypes = ::testing::Types<float, double, long double>;
TYPED_TEST_SUITE(BigintFloatInputTyped, FloatingTypes);

} // namespace

TYPED_TEST(BigintFloatInputTyped, TruncatesTowardZero) {
    using T = TypeParam;
    expectFloatValue(T{15.75}, "15");
    expectFloatValue(T{-15.75}, "-15");
    expectFloatValue(T{1.5}, "1");
    expectFloatValue(T{-1.5}, "-1");
    expectFloatValue(T{0.5}, "0");
    expectFloatValue(T{-0.5}, "0");
    expectFloatValue(T{0}, "0");
    expectFloatValue(-T{0}, "0");
    expectFloatValue(std::nextafter(T{1}, T{0}), "0");
    expectFloatValue(std::nextafter(T{-1}, T{0}), "0");
}

TYPED_TEST(BigintFloatInputTyped, AcceptsSmallestFiniteValuesAsZero) {
    using T = TypeParam;
    expectFloatValue(std::numeric_limits<T>::min(), "0");
    expectFloatValue(-std::numeric_limits<T>::min(), "0");
    if constexpr (std::numeric_limits<T>::has_denorm != std::denorm_absent) {
        expectFloatValue(std::numeric_limits<T>::denorm_min(), "0");
    }
}

TYPED_TEST(BigintFloatInputTyped, ConvertsExactPowersOfTwo) {
    using T = TypeParam;
    if constexpr (std::numeric_limits<T>::radix == 2) {
        constexpr int exponent = 100;
        static_assert(std::numeric_limits<T>::max_exponent > exponent);
        const T value = std::ldexp(T{1}, exponent);
        const std::string magnitude = decimalPowerOfTwo(exponent);
        expectFloatValue(value, magnitude);
        expectFloatValue(-value, "-" + magnitude);
    }
}

TYPED_TEST(BigintFloatInputTyped, MaximumFiniteValueHasExactFullDecimalDigits) {
    using T = TypeParam;
    const T maximum = std::numeric_limits<T>::max();
    std::vector<char> buffer(
        static_cast<std::size_t>(std::numeric_limits<T>::max_exponent10) + 4
    );
    const auto [end, ec] = std::to_chars(
        buffer.data(), buffer.data() + buffer.size(),
        maximum, std::chars_format::fixed, 0
    );
    ASSERT_EQ(ec, std::errc{});

    // The formatter provides reference digits for checking BigInt limb conversion.
    // Exact powers of two above have a separate, arithmetic-only reference.
    const std::string_view expected{
        buffer.data(), static_cast<std::size_t>(end - buffer.data())
    };
    ASSERT_FALSE(expected.empty());
    expectFloatValue(maximum, expected);
    expectFloatValue(-maximum, "-" + std::string(expected));
}

TYPED_TEST(BigintFloatInputTyped, NonFiniteInputsThrowWithoutMutatingDestination) {
    using T = TypeParam;
    if constexpr (std::numeric_limits<T>::has_infinity) {
        for (const T input : {
            std::numeric_limits<T>::infinity(),
            -std::numeric_limits<T>::infinity()
        }) {
            EXPECT_THROW((BigInt{input}), std::domain_error);
            BigInt destination;
            tester::load(destination, {77, 0, 0}, true);
            EXPECT_THROW(destination = input, std::domain_error);
            expectRepresentation(destination, {77, 0, 0}, true);
        }
    }
    if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
        const T input = std::numeric_limits<T>::quiet_NaN();
        EXPECT_THROW((BigInt{input}), std::domain_error);
        BigInt destination;
        tester::load(destination, {}, true);
        EXPECT_THROW(destination = input, std::domain_error);
        expectRepresentation(destination, {}, true);
    }
}

TEST(BigintFloatInput, Binary32StoredValueIsNotItsRoundedDecimalLiteral) {
    if constexpr (std::numeric_limits<float>::is_iec559 &&
                  std::numeric_limits<float>::radix == 2 &&
                  std::numeric_limits<float>::digits == 24) {
        expectFloatValue(1e20f, "100000002004087734272");
        expectFloatValue(-1e20f, "-100000002004087734272");
    }
}

TEST(BigintFloatInput, BinaryPrecisionBoundaryValues) {
    if constexpr (std::numeric_limits<float>::is_iec559 &&
                  std::numeric_limits<float>::radix == 2 &&
                  std::numeric_limits<float>::digits == 24) {
        expectFloatValue(16777216.0f, "16777216");
        expectFloatValue(16777218.0f, "16777218");
    }
    if constexpr (std::numeric_limits<double>::is_iec559 &&
                  std::numeric_limits<double>::radix == 2 &&
                  std::numeric_limits<double>::digits == 53) {
        expectFloatValue(9007199254740992.0, "9007199254740992");
        expectFloatValue(9007199254740994.0, "9007199254740994");
    }
}
