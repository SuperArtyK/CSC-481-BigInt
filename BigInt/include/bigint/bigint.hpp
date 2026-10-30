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


inline BigInt::BigInt(): 
	m_bNegative(false) {
	cdbg<<"Constructing empty bigint"<<NLC;
}

inline BigInt::BigInt(const BigInt& bint): 
	m_lstDigits(bint.m_lstDigits), m_bNegative(bint.m_bNegative) {
	cdbg<<"Constructing from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
}

inline BigInt::BigInt(BigInt&& bint) noexcept {
	cdbg<<"Moving from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = std::move(bint.m_lstDigits);
	this->m_bNegative = bint.m_bNegative;
}

inline BigInt& BigInt::operator=(const BigInt& bint) {
	cdbg<<"Assigning from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = bint.m_lstDigits;
	this->m_bNegative = bint.m_bNegative;
	return *this;
}

inline BigInt& BigInt::operator=(BigInt&& bint) noexcept {
	cdbg<<"Move-assignment from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = std::move(bint.m_lstDigits);
	this->m_bNegative = bint.m_bNegative;
	return *this;
}



#endif //CSC_481_BIGINT_BIGINT_HPP
