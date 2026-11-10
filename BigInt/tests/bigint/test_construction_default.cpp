//
// Created by artyk on 11/7/26.
//

#include <gtest/gtest.h>
#include "bigint/bigint.hpp"
#include "test_helpers.hpp"

using tester = bigint_test::Inspector;
using bigint_test::expectRepresentation;

TEST(BigintConstruction, DefaultZero) {
	BigInt a;

	expectRepresentation(a, {0}, false);

}
