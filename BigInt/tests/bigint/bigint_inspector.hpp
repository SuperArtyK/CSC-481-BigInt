//
// Created by artyk on 11/7/26.
//

#ifndef CSC_481_BIGINT_BIGINT_INSPECTOR_HPP
#define CSC_481_BIGINT_BIGINT_INSPECTOR_HPP

#include "bigint/bigint.hpp"
#include <vector>
#include <initializer_list>

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
	
}


#endif //CSC_481_BIGINT_BIGINT_INSPECTOR_HPP
