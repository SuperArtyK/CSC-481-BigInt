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

/// @brief Test support utilities for validating BigInt internals and copy/move semantics.
/// @details This namespace contains helpers used by unit tests to inspect private representation details,
///          assert expected storage layouts, and confirm that assignment operations preserve the value
///          without changing internal sign or limb ordering.
namespace bigint_test {

	/// @brief Test helper for inspecting the private internal representation of a BigInt.
	/// @note This is intentionally a test-only bridge that exposes internals so unit tests can verify
	///       normalization, sign handling, and storage layout without relying on user-facing behavior.
	struct Inspector {

		/// @brief Returns the raw limb storage of the bigint in least-significant-first order.
		/// @param bint BigInt instance to inspect.
		/// @return Vector containing the stored magnitude limbs.
		static std::vector<limb_t> getLimbs(const BigInt& bint) {
			return {bint.m_lstDigits.begin(), bint.m_lstDigits.end()};
		}

		/// @brief Returns the sign flag stored in the bigint.
		/// @param bint BigInt instance to inspect.
		/// @return True if the value is negative, false otherwise.
		static bool getNegative(const BigInt& bint) {
			return bint.m_bNegative;
		}

		/// @brief Loads a bigint directly with a supplied internal limb list and sign.
		/// @param bint BigInt instance to populate.
		/// @param limbs Sequence of magnitude limbs in least-significant-first order.
		/// @param neg Sign flag to assign to the bigint.
		static void load(BigInt& bint, const std::initializer_list<limb_t>& limbs, const bool neg) {
			bint.m_lstDigits.assign(limbs);
			bint.m_bNegative = neg;
		}

	};

	/// @brief Asserts that a bigint matches the expected internal representation.
	/// @param value BigInt instance under test.
	/// @param expectedLimbs Expected limb sequence in least-significant-first order.
	/// @param expectedNegative Expected value of the sign flag.
	inline void expectRepresentation(
		const BigInt& value,
		const std::vector<limb_t>& expectedLimbs,
		const bool expectedNegative
	) {
		EXPECT_EQ(Inspector::getLimbs(value), expectedLimbs);
		EXPECT_EQ(Inspector::getNegative(value), expectedNegative);
	}

	/// @brief Verifies that copy-assignment preserves the original internal representation.
	/// @param sourceLimbs Initial limb sequence for the source bigint.
	/// @param sourceNegative Initial sign state of the source bigint.
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

	/// @brief Verifies that move-assignment preserves the source state while transferring ownership.
	/// @param sourceLimbs Initial limb sequence for the source bigint.
	/// @param sourceNegative Initial sign state of the source bigint.
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
