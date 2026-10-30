//
// Created by artyk on 10/30/26.
//

#include "bigint/bigint.hpp"


BigInt::BigInt(): 
	m_bNegative(false) {
	cdbg<<"Constructing empty bigint"<<NLC;
}

BigInt::BigInt(const BigInt& bint): 
	m_lstDigits(bint.m_lstDigits), m_bNegative(bint.m_bNegative) {
	cdbg<<"Constructing from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
}

BigInt::BigInt(BigInt&& bint) noexcept {
	cdbg<<"Moving from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = std::move(bint.m_lstDigits);
	this->m_bNegative = bint.m_bNegative;
}

BigInt& BigInt::operator=(const BigInt& bint) {
	cdbg<<"Assigning from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = bint.m_lstDigits;
	this->m_bNegative = bint.m_bNegative;
	return *this;
}

BigInt& BigInt::operator=(BigInt&& bint) noexcept {
	cdbg<<"Move-assignment from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	this->m_lstDigits = std::move(bint.m_lstDigits);
	this->m_bNegative = bint.m_bNegative;
	return *this;
}
