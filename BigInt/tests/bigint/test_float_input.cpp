#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using bigint_test::expectRepresentation;
using tester = bigint_test::Inspector;

namespace {

// Independent decimal arithmetic for powers and floating-point maxima.
// Only integer digits are used: no floating-point formatting or BigInt code.
void multiplyDecimal(std::string& digits, const int factor) {
    int carry = 0;
    for (std::size_t i = digits.size(); i-- > 0;) {
        const int product = (digits[i] - '0') * factor + carry;
        digits[i] = static_cast<char>('0' + product % 10);
        carry = product / 10;
    }
    while (carry != 0) {
        digits.insert(digits.begin(), static_cast<char>('0' + carry % 10));
        carry /= 10;
    }
}

void decrementDecimal(std::string& digits) {
    for (std::size_t i = digits.size(); i-- > 0;) {
        if (digits[i] != '0') {
            --digits[i];
            break;
        }
        digits[i] = '9';
    }
    if (digits.front() == '0' && digits.size() > 1) {
        digits.erase(0, 1);
    }
}

std::string decimalPower(const int radix, const int exponent) {
    std::string digits = "1";
    for (int i = 0; i < exponent; ++i) {
        multiplyDecimal(digits, radix);
    }
    return digits;
}

// A maximum finite radix-r floating-point value is
// (r^digits - 1) * r^(max_exponent - digits).
// Return its integer part without invoking std::to_chars or BigInt.
template <typename T>
std::string maximumFiniteIntegerText() {
    using Limits = std::numeric_limits<T>;
    static_assert(Limits::radix >= 2);

    if constexpr (Limits::max_exponent <= 0) {
        return "0";
    } else if constexpr (Limits::max_exponent < Limits::digits) {
        // Maximum lies strictly between r^E - 1 and r^E.
        std::string digits = decimalPower(Limits::radix, Limits::max_exponent);
        decrementDecimal(digits);
        return digits;
    } else {
        std::string digits = decimalPower(Limits::radix, Limits::digits);
        decrementDecimal(digits);
        for (int i = Limits::digits; i < Limits::max_exponent; ++i) {
            multiplyDecimal(digits, Limits::radix);
        }
        return digits;
    }
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
        const std::string magnitude = decimalPower(2, exponent);
        expectFloatValue(value, magnitude);
        expectFloatValue(-value, "-" + magnitude);
    }
}

TYPED_TEST(BigintFloatInputTyped, MaximumFiniteValueHasExactFullDecimalDigits) {
    using T = TypeParam;
    using Limits = std::numeric_limits<T>;

    const std::string expected = maximumFiniteIntegerText<T>();

    // copyFromFloat allocates max_exponent10 + 3 bytes and reserves one
    // byte in the to_chars output range. The maximum must still fit.
    constexpr std::size_t capacity =
        static_cast<std::size_t>(Limits::max_exponent10) + 3;
    ASSERT_LE(expected.size(), capacity - 1);

    // The maximum finite value also exercises the longest integer text
    // available for this type's fixed-decimal conversion.
    EXPECT_EQ(expected.size(), static_cast<std::size_t>(Limits::max_exponent10) + 1);

    expectFloatValue(Limits::max(), expected);
    expectFloatValue(-Limits::max(), "-" + expected);
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
