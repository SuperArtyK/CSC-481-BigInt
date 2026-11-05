//
// Created by artyk on 11/1/26.
//

#include "bigint/bigint_declaration.hpp"
#include "bigint/bigint_accessors.hpp"

bool BigInt::setLimb(const std::size_t index, const limb_t value) {
		
	if (value != 0 || index <= this->getLimbCount()) {
		this->setLimbRaw(index, value);
	}
		
	this->normalise();
		
	return true; // succeeded
}

bool BigInt::setLimbRaw(const std::size_t index, const limb_t value) {
	// value is more than what can be stored
	if constexpr (std::is_signed<limb_t>::value) {
		if (value < 0) {
			return false;
		}
	}
	
	if (value > STORAGE_MAX) {
		return false; // failed
	}
		
	// index is larger than what can be stored
	if (index >= this->getLimbCount()) {
		this->m_lstDigits.resize(index + 1);
	}
	
	this->m_lstDigits[index] = value;
		
	return true; // succeeded
}

std::string BigInt::toStringDec() const {
		
	if (this->isZero()) {
		return "0";
	}
		
	//idkman, I wish I could just specify the size withouth the char
	std::string result;
	result.reserve(this->isNegative() + this->getDigitCount());
		
	if (this->isNegative()) {
		result.push_back('-');
	}
		
	char numBuf[LIMB_DIGITS + 1];
	std::size_t len = 0;
		
	//first time without zero-leading logic
	result.append(
		numBuf, 
		static_cast<std::size_t>(
			std::to_chars(numBuf, numBuf+sizeof(numBuf)-1, this->getLastLimb()).ptr - numBuf
		)
	);
		
	for (std::size_t i = this->getLimbCount()-1; i-- > 0;) {
		//format, get length
		len = static_cast<std::size_t>(
			std::to_chars(numBuf, numBuf+sizeof(numBuf)-1, this->getLimb(i)).ptr - numBuf
		);
			
		result.append(LIMB_DIGITS-len, '0');
		result.append(numBuf, len);
	}
		
	return result;
}



void BigInt::normalise() {
	if (this->getLimbCount() == 0) {
		this->setToZero();
	}
		
	while (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		this->m_lstDigits.pop_back();
	}
		
	if (this->isZero()) {
		this->setNegative(false);
	}
}

bool BigInt::isNormalised() const noexcept {
	// empty deque
	if (this->getLimbCount() == 0) {
		return false;
	}
		
	// leading 0's
	if (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		return false;
	}
		
	//
	if (this->isZero() && this->isNegative()) {
		return false;
	}
		
	return true;
}