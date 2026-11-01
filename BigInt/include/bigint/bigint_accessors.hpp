//
// Created by artyk on 10/31/26.
//

#ifndef CSC_481_BIGINT_BIGINT_ACCESSORS_HPP
#define CSC_481_BIGINT_BIGINT_ACCESSORS_HPP

#include "bigint_declaration.hpp"


inline void BigInt::setToZero(const bool shrink) {
	this->clear(shrink);
	this->m_lstDigits.push_back(0);
}

inline std::size_t BigInt::getLimbCount() const noexcept {
	return this->m_lstDigits.size();
}

inline std::uintmax_t BigInt::getDigitCount() const noexcept {
	return (this->getLimbCount()-1) * LIMB_DIGITS + digitCount(this->getLastLimb());
}

inline limb_t BigInt::getLimb(const std::size_t index) const noexcept {
	if (index >= this->m_lstDigits.size()) {
		return 0;
	}
	return this->m_lstDigits[index];
}

inline limb_t BigInt::getLastLimb() const noexcept {
	return this->m_lstDigits.back();
}

inline limb_t BigInt::getFirstLimb() const noexcept {
	return this->m_lstDigits.front();
}

inline bool BigInt::isNegative() const noexcept {
	return this->m_bNegative;
}

inline bool BigInt::isZero() const noexcept {
	return this->getLimbCount() == 1 && this->getFirstLimb() == 0;
}

inline void BigInt::setNegative(const bool neg) {
	this->m_bNegative = neg;
}


inline void BigInt::clear(const bool shrink) {
	this->m_lstDigits.clear();
	if (shrink) {
		this->m_lstDigits.shrink_to_fit();
	}
	this->m_bNegative = false;
}




#endif //CSC_481_BIGINT_BIGINT_ACCESSORS_HPP
