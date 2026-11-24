//
// Created by artyk on 11/1/26.
//

#include "bigint/bigint.hpp"
#include <limits>
#include <stdexcept>

bool BigInt::setLimb(const std::size_t index, const limb_t value) {
	if (index >= this->getLimbCount() && value == 0) {
		return true; // no-op
	}

	if (!this->setLimbRaw(index, value)) {
		return false;
	}

	cdbg << "Normalising after setting the limb value" << NLC;
	this->normalise();

	return true; // succeeded
}

bool BigInt::setLimbRaw(const std::size_t index, const limb_t value) {
	cdbg << "Setting limb at index " << index << " to value " << value << NLC;
	// value is more than what can be stored
	if constexpr (std::is_signed<limb_t>::value) {
		if (value < 0) {
			cdbg << "Value is less than 0 (signed limb_t), failed" << NLC;
			return false;
		}
	}

	if (value > STORAGE_MAX) {
		cdbg << "Value is more then STORAGE_MAX, failed" << NLC;
		return false; // failed
	}

	if (index == std::numeric_limits<std::size_t>::max()) {
		throw std::length_error("BigInt limb index exceeds maximum storage size");
	}

	// index is larger than what can be stored
	if (index >= this->getLimbCount()) {
		cdbg << "Index is >= than current limb count " << this->getLimbCount() << "; resizing..." << NLC;
		this->m_lstDigits.resize(index + 1, 0);
	}

	this->m_lstDigits[index] = value;

	return true; // succeeded
}

std::string BigInt::toStringDec() const {
	if (!this->isNormalised()) {
		throw std::logic_error("BigInt must be normalised");
	}
	
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
			std::to_chars(numBuf, numBuf + sizeof( numBuf ) - 1, this->getLastLimb()).ptr - numBuf
		)
	);

	for (std::size_t i = this->getLimbCount() - 1; i-- > 0;) {
		//format, get length
		len = static_cast<std::size_t>(
			std::to_chars(numBuf, numBuf + sizeof( numBuf ) - 1, this->getLimb(i)).ptr - numBuf
		);

		result.append(LIMB_DIGITS - len, '0');
		result.append(numBuf, len);
	}

	return result;
}

std::string BigInt::toStringSci(const std::size_t maxSigDig, const bool pad) const {
	if (!this->isNormalised()) {
		throw std::logic_error("BigInt must be normalised");
	}

	if (maxSigDig == 0) {
		throw std::invalid_argument("Precision must be positive");
	}

	std::string result = this->toStringDec();

	// Special handling for zero.
	if (result == "0") {
		if (maxSigDig > 1) {
			result.push_back('.');
			result.append(maxSigDig - 1, '0');
		}

		return result + "e+0";
	}

	const std::size_t signOffset = this->isNegative() ? 1 : 0;
	const std::size_t digitCount = result.size() - signOffset;
	const bool needsRounding = digitCount > maxSigDig;

	std::size_t exponent = digitCount - 1;

	if (needsRounding) {
		// Save the first discarded digit before truncating.
		const char roundingDigit = result[signOffset + maxSigDig];
		result.resize(signOffset + maxSigDig);

		// Round to nearest, ties away from zero.
		if (roundingDigit >= '5') {
			std::size_t i = result.size();

			// Propagate carry through trailing nines.
			while (i > signOffset && result[i - 1] == '9') {
				result[--i] = '0';
			}

			if (i == signOffset) {
				// All retained digits were nines.
				result[signOffset] = '1';
				++exponent;
			}
			else {
				++result[i - 1];
			}
		}

		// Remove trailing zeroes only if digits were discarded.
		if (!pad) {
			while (result.size() > signOffset + 1 &&
				result.back() == '0') {
				result.pop_back();
				}
		}
	}
	else if (pad) {
		result.append(maxSigDig - digitCount, '0');
	}

	// Insert decimal point after the first significant digit.
	if (result.size() > signOffset + 1) {
		result.insert(signOffset + 1, 1, '.');
	}

	result += "e+";
	result += std::to_string(exponent);

	return result;
}


void BigInt::normalise() {
	if (this->getLimbCount() == 0) {
		cdbg << "Bigint has no limbs, setting it to 0" << NLC;
		this->setToZero();
	}
#if DEBUGMODE
	if (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		cdbg << "Non-first limb equals to 0; launching the pop_back loop" << NLC;
	}
#endif


	while (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
		this->m_lstDigits.pop_back();
	}

	if (this->isZero()) {
		cdbg << "number's magnitude is zero; setting negativity to false (just in case lol)" << NLC;
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
