//
// Created by artyk on 10/30/26.
//

#include "bigint/bigint.hpp"


BigInt::BigInt(): 
	m_bNegative(false) {
	cdbg<<"Constructing empty bigint"<<NLC;
	this->setToZero(false);
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


bool BigInt::setLimb(const std::size_t index, const limb_t value) {
		
	// value is more than what can be stored
	if (value > STORAGE_MAX) {
		return false; // failed
	}
		
	// index is larger than what can be stored
	if (index >= this->getLimbCount()) {
		this->m_lstDigits.resize(index + 1);
	}
		
	// index is exactly the last limb
	if (value == 0 && index == this->getLimbCount() -1 ) {
		// index is exactly the first limb too
		if (index == 0) {
			this->setToZero(false); // then set to zero
			return true;
		}
		// exactly the last -> delete the last one
		this->m_lstDigits.pop_back();
	}
		
	this->m_lstDigits[index] = value;
		
	return true; // succeeded
}
