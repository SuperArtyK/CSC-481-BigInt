//
// Created by artyk on 11/1/26.
//

#include "bigint/bigint_declaration.hpp"
#include "bigint/bigint_accessors.hpp"
#include "bigint/utils.hpp"
#include <limits>
#include <stdexcept>

bool BigInt::setLimb(const std::size_t index, const limb_t value) {
	if (index >= this->getLimbCount() && value == 0) {
		return true; // no-op
	}
	
	if (!this->setLimbRaw(index, value)) {
		return false;
	}
		
	cdbg<<"Normalising after setting the limb value"<<NLC;
	this->normalise();
		
	return true; // succeeded
}

bool BigInt::setLimbRaw(const std::size_t index, const limb_t value) {
	cdbg<<"Setting limb at index "<<index<<" to value "<<value<<NLC;
	// value is more than what can be stored
	if constexpr (std::is_signed<limb_t>::value) {
		if (value < 0) {
			cdbg<<"Value is less than 0 (signed limb_t), failed"<<NLC;
			return false;
		}
	}
	
	if (value > STORAGE_MAX) {
		cdbg<<"Value is more then STORAGE_MAX, failed"<<NLC;
		return false; // failed
	}

	if (index == std::numeric_limits<std::size_t>::max()) {
		throw std::length_error("BigInt limb index exceeds maximum storage size");
	}
		
	// index is larger than what can be stored
	if (index >= this->getLimbCount()) {
		cdbg<<"Index is >= than current limb count "<<this->getLimbCount()<<"; resizing..."<<NLC;
		this->m_lstDigits.resize(index + 1, 0);
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
		cdbg<<"Bigint has no limbs, setting it to 0" <<NLC;
		this->setToZero();
	}
#ifdef DEBUGMODE
	if (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		cdbg<<"Non-first limb equals to 0; launching the pop_back loop"<<NLC;
	}
#endif
	
		
	while (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		this->m_lstDigits.pop_back();
	}
		
	if (this->isZero()) {
		cdbg<<"number's magnitude is zero; setting negativity to false (just in case lol)"<<NLC;
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