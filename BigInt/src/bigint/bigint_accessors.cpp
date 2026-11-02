//
// Created by artyk on 11/1/26.
//

#include "bigint/bigint_declaration.hpp"

bool BigInt::setLimb(const std::size_t index, const limb_t value) {
		
	// value is more than what can be stored
	if (value > STORAGE_MAX) {
		return false; // failed
	}
		
	// index is larger than what can be stored
	if (index >= this->getLimbCount()) {
		if (value == 0) {
			this->normalise();
			return true;
		}
		this->m_lstDigits.resize(index + 1);
	}
	
	this->m_lstDigits[index] = value;
	
	this->normalise();
		
	return true; // succeeded
}


inline std::string BigInt::toStringDec() const {
		
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