//
// Created by artyk on 10/30/26.
//

#ifndef CSC_481_BIGINT_BIGINT_HPP
#define CSC_481_BIGINT_BIGINT_HPP

#include "utils.hpp"
#include <deque>
#include <climits>
#include <limits>
#include <cstring>

typedef std::uint64_t limb_t;

class BigInt {
	
public:
	
	BigInt();

	BigInt(const BigInt& bint);

	BigInt(BigInt&& bint) noexcept;

	BigInt& operator=(const BigInt& bint);

	BigInt& operator=(BigInt&& bint) noexcept;

private:
	
	std::deque<limb_t> m_lstDigits;
	bool m_bNegative;
	
	
};



#endif //CSC_481_BIGINT_BIGINT_HPP
