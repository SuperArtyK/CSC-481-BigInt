#include <gtest/gtest.h>

#include <array>
#include <string>
#include <string_view>

#include "bigint/utils.hpp"

namespace {

using Status = StrValidation::ValStatus;

struct ValidCase {
    std::string_view input;
    std::string_view integer;
    std::string_view fraction;
    bool negative;
    bool nonZeroFraction;
};

} // namespace

TEST(StringValidation, ValidDecimalComponents) {
    constexpr std::array cases{
        ValidCase{"0", "0", "", false, false},
        ValidCase{"-0", "0", "", true, false},
        ValidCase{"+000123", "000123", "", false, false},
        ValidCase{"  -00123.4500\t", "00123", "4500", true, true},
        ValidCase{".5", "", "5", false, true},
        ValidCase{"-.000", "", "000", true, false},
        ValidCase{"5.", "5", "", false, false},
        ValidCase{"+.7", "", "7", false, true},
        ValidCase{"123.000", "123", "000", false, false},
        ValidCase{"123.0010", "123", "0010", false, true},
        ValidCase{"\t 42 \t", "42", "", false, false},
    };

    for (const auto& testCase : cases) {
        SCOPED_TRACE(::testing::PrintToString(testCase.input));
        const auto result = strIsNum(testCase.input);
        EXPECT_EQ(result.status, Status::Valid);
        EXPECT_EQ(result.intPart, testCase.integer);
        EXPECT_EQ(result.decPart, testCase.fraction);
        EXPECT_EQ(result.negative, testCase.negative);
        EXPECT_EQ(result.hasNonZeroFraction, testCase.nonZeroFraction);
    }
}

TEST(StringValidation, EmptyInputIsDistinctFromInvalidInput) {
    constexpr std::array<std::string_view, 4> cases{
        "", " ", "\t", " \t \t "
    };

    for (const auto input : cases) {
        SCOPED_TRACE(::testing::PrintToString(input));
        const auto result = strIsNum(input);
        EXPECT_EQ(result.status, Status::Empty);
        EXPECT_TRUE(result.intPart.empty());
        EXPECT_TRUE(result.decPart.empty());
        EXPECT_FALSE(result.negative);
        EXPECT_FALSE(result.hasNonZeroFraction);
    }
}

TEST(StringValidation, MalformedTextResetsAllResultFields) {
    constexpr std::array<std::string_view, 25> cases{
        "+", "-", ".", "+.", "-.", "++1", "--1", "+-1", "-+1",
        "1.2.3", "1..", "1e3", "1E3", "1,000", "0x10", "12x", "x12",
        "1 2", "1\t2", "\n1", "1\r", "\f", "\v", "\n", "1\n2"
    };

    for (const auto input : cases) {
        SCOPED_TRACE(::testing::PrintToString(input));
        const auto result = strIsNum(input);
        EXPECT_EQ(result.status, Status::Invalid);
        EXPECT_TRUE(result.intPart.empty());
        EXPECT_TRUE(result.decPart.empty());
        EXPECT_FALSE(result.negative);
        EXPECT_FALSE(result.hasNonZeroFraction);
    }
}

TEST(StringValidation, RejectsEmbeddedNullAndNonAsciiBytes) {
    const std::string embeddedNull{"12\0.3", 5};
    const std::string nonAscii{"1\xC2\xA0", 3};

    EXPECT_EQ(strIsNum(std::string_view{embeddedNull}).status, Status::Invalid);
    EXPECT_EQ(strIsNum(std::string_view{nonAscii}).status, Status::Invalid);
}

TEST(StringValidation, ReturnedViewsReferToTheOriginalStorage) {
    const std::string input = " \t-0012.034 \t";
    const auto result = strIsNum(input);

    ASSERT_EQ(result.status, Status::Valid);
    EXPECT_EQ(result.intPart, "0012");
    EXPECT_EQ(result.decPart, "034");
    EXPECT_EQ(result.intPart.data(), input.data() + 3);
    EXPECT_EQ(result.decPart.data(), input.data() + 8);
}

TEST(StringValidation, RespectsStringViewLengthWithoutNullTermination) {
    const std::string backing = "x-12.75z";
    const std::string_view slice{backing.data() + 1, 6};
    const auto result = strIsNum(slice);

    ASSERT_EQ(result.status, Status::Valid);
    EXPECT_EQ(result.intPart, "12");
    EXPECT_EQ(result.decPart, "75");
    EXPECT_TRUE(result.negative);
}
