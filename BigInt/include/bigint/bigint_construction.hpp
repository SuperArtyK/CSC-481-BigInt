//
// Created by artyk on 11/16/26.
//

#ifndef CSC_481_BIGINT_BIGINT_CONSTRUCTION_HPP
#define CSC_481_BIGINT_BIGINT_CONSTRUCTION_HPP

#include "bigint_declaration.hpp"

// all template stuff goes here


template <std::integral T>
BigInt::BigInt(const T num): m_bNegative(false) {
	cdbg<<"Constructing from an integral type: "<<+num<<NLC;
	this->copyFromInt(num);
}

template <std::integral T>
BigInt& BigInt::operator=(const T num) {
	this->copyFromInt(num);
	return *this;
}

template <std::integral T>
void BigInt::copyFromInt(const T num) {
	//clear the bigint, as always
	this->clear(false);

	// Riiiight, handling bools with unsigned is ill-formed
	// I need to handle them separately
	// don't forget the "remove const-volatile" or it won't proceed!
	if constexpr (std::is_same<typename std::remove_cv<T>::type, bool>::value) {
		this->m_lstDigits.push_back(num);
	}
	else {
		//'kay, actual logic is here

		//unsigned type to handle all
		using unsigned_t = std::make_unsigned<T>::type;

		unsigned_t unum;
	
		// just in case the value could be negative
		if constexpr (std::is_signed<T>::value) {
			this->m_bNegative = ( num < 0 );
			unum = ( num < 0 ) ? unsigned_t(0) - static_cast<unsigned_t>(num) : static_cast<unsigned_t>(num);
		}
		else {
			unum = static_cast<unsigned_t>(num);
		}

		// and if it's bigger than a single limb
		if constexpr (std::numeric_limits<T>::max() > STORAGE_MAX) {
			if (unum > STORAGE_MAX) {
				while (unum > 0) {
					this->m_lstDigits.push_back(unum % STORAGE_MASK);
					unum /= STORAGE_MASK;
				}
			}
			else {
				//awww, I don't wanna repeat codeee
				this->m_lstDigits.push_back(unum);
			}
		}
		else {
			this->m_lstDigits.push_back(unum);
		}
	}
}


#endif //CSC_481_BIGINT_BIGINT_CONSTRUCTION_HPP
