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
#include <cstdint>
#include <cmath>
#include <cinttypes>
#include <exception>
#include <ranges>

/// @brief The underlying type used to store digits in a bigint
/// @see BigInt
typedef std::uint64_t limb_t;

/// @brief Maximum allowed digits to store inside one limb
/// @see limb_t
constexpr int LIMB_DIGITS = std::numeric_limits<limb_t>::digits10 - 1;

/// @brief List of powers of 10 that would be used when working with the limb type.
/// @todo Create a system to fill the table programmatically for each type
/// @warning This table is only valid for std::uint64_t
constexpr limb_t powersOf10[]{
	1,
	10,
	100,
	1000,
	10000,
	100000,
	1000000,
	10000000,
	100000000,
	1000000000,
	10000000000,
	100000000000,
	1000000000000,
	10000000000000,
	100000000000000,
	1000000000000000,
	10000000000000000,
	100000000000000000,
	1000000000000000000,
	10000000000000000000
};


constexpr limb_t STORAGE_MASK = powersOf10[LIMB_DIGITS];

constexpr limb_t STORAGE_MAX = STORAGE_MASK - 1;


/// @brief Big integer type class, copies the functionality of regular integers, but on arbitrary scale.
/// @note Technical limit for the size of the BigInt is std::size_t*digits(limb_t)
/// @see limb_t
/// @see limbDigits
class BigInt {
public:
	
	/// @brief Default constructor for the class
	BigInt();
	
	/// @brief Copy-constructor for the class
	/// @param bint the bigint to copy values from
	BigInt(const BigInt& bint);

	/// @brief Move-constructor for the class
	/// @param bint the bigint to move values from
	BigInt(BigInt&& bint) noexcept;
	
	
	/// @brief Copy-assignment for the class
	/// @param bint the bigint to copy values from
	BigInt& operator=(const BigInt& bint);
	
	/// @brief Move-assignment for the class
	/// @param bint the bigint to move values from
	BigInt& operator=(BigInt&& bint) noexcept;
	
	/// @brief Set the underlying bigint to 0
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	inline void setToZero(const bool shrink) {
		this->clear(shrink);
	}
	
	
	[[nodiscard]] inline std::size_t getLimbCount() const noexcept {
		return this->m_lstDigits.size();
	}
	
	inline std::uintmax_t getDigitCount() const noexcept {
		return this->getLimbCount() * LIMB_DIGITS + digitCount(this->getLastLimb());
	}
	
	inline limb_t getLimb(const std::size_t index) const noexcept {
		if (index >= this->m_lstDigits.size()) {
			return 0;
		}
		return this->m_lstDigits[index];
	}
	
	inline limb_t getLastLimb() const noexcept {
		return *(this->m_lstDigits.end());
	}
	
	inline limb_t getFirstLimb() const noexcept {
		return *(this->m_lstDigits.begin());
	}
	
	
	inline bool isNegative() const noexcept {
		return this->m_bNegative;
	}
	 
	inline bool isZero() const noexcept {
		return this->getLimbCount() == 1 && this->getFirstLimb() == 0;
	}
	
	bool setLimb(const std::size_t index, const limb_t value) {
		
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
			this->m_lstDigits.erase(this->m_lstDigits.end());
		}
		
		this->m_lstDigits[index] = value;
		
		return true; // succeeded
	}
	
	
	
private:
	
	/// @brief Completely clear the underlying bigint: clear containers and remove negativity
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	void clear(const bool shrink) {
		this->m_lstDigits.clear();
		if (shrink) {
			this->m_lstDigits.shrink_to_fit();
		}
		this->m_bNegative = false;
	}
	
	/// Storage for the limbs
	/// @see limb_t
	std::deque<limb_t> m_lstDigits;
	
	/// Flag to indicate whether the bigint is negative
	bool m_bNegative;
};


#endif //CSC_481_BIGINT_BIGINT_HPP
