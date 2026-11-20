//
// Created by artyk on 10/30/26.
//

#include "bigint/bigint.hpp"
#include <stdexcept>

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

BigInt::BigInt(const std::string_view str) {
	cdbg<<"Constructing from std::string_view; size: "<<str.size()<<NLC;
	this->copyFromString(str);
}

BigInt& BigInt::operator=(const BigInt& bint) {
	cdbg<<"Assigning from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	if (this == &bint) {
		cdbg<<"WARNING: Self assignment detected! Returning early..."<<NLC;
		return *this;
	}
	this->m_lstDigits = bint.m_lstDigits;
	this->m_bNegative = bint.m_bNegative;
	return *this;
}

BigInt& BigInt::operator=(BigInt&& bint) noexcept {
	cdbg<<"Move-assignment from another bigint, limbs: "<<bint.m_lstDigits.size()<<", is negative:"<<boolToString(bint.m_bNegative)<<NLC;
	if (this == &bint) {
		cdbg<<"WARNING: Self assignment detected! Returning early..."<<NLC;
		return *this;
	}
	this->m_lstDigits = std::move(bint.m_lstDigits);
	this->m_bNegative = bint.m_bNegative;
	return *this;
}

BigInt& BigInt::operator=(const std::string_view str) {
	cdbg<<"Assigning from std::string_view; size: "<<str.size()<<NLC;
	this->copyFromString(str);
	return *this;
}

void BigInt::copyFromString(const std::string_view str) {
	StrValidation result = strIsNum(str);

	if (result.status == StrValidation::ValStatus::Invalid) {
		cdbg<<"Passed string is sadly invalid"<<NLC;
		throw std::invalid_argument{"Passed string is not a number!"};
	}
	
	this->copyFromValidString(result);
}

void BigInt::copyFromValidString(const StrValidation& result) {
	cdbg<<"Copying values from the validated string."<<NLC;
	cdbg<<"Integer part size: "<<result.intPart.size()<<"; Fraction part size:"<<result.decPart.size()
		<<"; Negative: "<<boolToString(result.negative)<<"; Fractional part is non-zero: "<<boolToString(result.hasNonZeroFraction)<<NLC;
	
	if (result.status == StrValidation::ValStatus::Empty) {
		cdbg<<"Passed string is empty. Setting bigint to 0 instead"<<NLC;
		this->setToZero();
		return;
	}
	
	
	std::string_view str = result.intPart;

	// Remove unnecessary leading zeros.
	const std::size_t first = str.find_first_not_of('0');
	
	// Empty integer portion or zero magnitude.
	if (first == std::string_view::npos) {
		cdbg<<"String is empty after clearing leading 0's. Setting bigint to 0"<<NLC;
		this->setToZero();
		return;
	}

	str.remove_prefix(first);

	// Calculate the required number of limbs.
	const std::size_t limbCount = 1 + ( str.size() - 1 ) / LIMB_DIGITS;
	cdbg<<"Calculated limb count: "<<limbCount<<NLC;
	
	// Resize storage once.
	this->m_lstDigits.resize(limbCount);

	// Convert decimal groups from least to most significant.
	std::size_t end = str.size();

	for (std::size_t i = 0; i < limbCount; ++i) {
		const std::size_t start = end > LIMB_DIGITS ? end - LIMB_DIGITS : 0;

		limb_t value = 0;

		for (std::size_t j = start; j < end; ++j) {
			value = value * 10 + static_cast<limb_t>(str[j] - '0');
		}

		this->m_lstDigits[i] = value;
		end = start;
	}

	// Set the sign after successful magnitude construction.
	this->m_bNegative = result.negative;
}