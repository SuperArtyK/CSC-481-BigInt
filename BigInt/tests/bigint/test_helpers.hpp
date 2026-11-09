//
// Created by artyk on 11/7/26.
//

#ifndef CSC_481_BIGINT_TEST_HELPERS_HPP
#define CSC_481_BIGINT_TEST_HELPERS_HPP

#include <gtest/gtest.h>
#include <initializer_list>
#include <utility>
#include <vector>
#include "bigint/bigint.hpp"

namespace bigint_test {

	struct Inspector {

		static std::vector<limb_t> getLimbs(const BigInt& bint) {
			return {bint.m_lstDigits.begin(), bint.m_lstDigits.end()};
		}

		static bool getNegative(const BigInt& bint) {
			return bint.m_bNegative;
		}

		static void load(BigInt& bint, const std::initializer_list<limb_t>& limbs, const bool neg) {
			bint.m_lstDigits.assign(limbs);
			bint.m_bNegative = neg;
		}

	};

	inline void expectRepresentation(
		const BigInt& value,
		const std::vector<limb_t>& expectedLimbs,
		const bool expectedNegative
	) {
		EXPECT_EQ(Inspector::getLimbs(value), expectedLimbs);
		EXPECT_EQ(Inspector::getNegative(value), expectedNegative);
	}

	inline void testCopyPreservesRepresentation(
		const std::initializer_list<limb_t>& sourceLimbs,
		const bool sourceNegative
	) {
		BigInt source;
		BigInt destination;
		Inspector::load(source, sourceLimbs, sourceNegative);

		destination = source;

		const std::vector<limb_t> expectedLimbs(sourceLimbs);
		expectRepresentation(destination, expectedLimbs, sourceNegative);
		expectRepresentation(source, expectedLimbs, sourceNegative);
	}

	inline void testMovePreservesRepresentation(
		const std::initializer_list<limb_t>& sourceLimbs,
		const bool sourceNegative
	) {
		BigInt source;
		BigInt destination;
		Inspector::load(source, sourceLimbs, sourceNegative);

		destination = std::move(source);

		expectRepresentation(destination, std::vector<limb_t>(sourceLimbs), sourceNegative);
	}

}


#endif //CSC_481_BIGINT_TEST_HELPERS_HPP
