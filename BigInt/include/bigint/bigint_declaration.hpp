//
// Created by artyk on 10/31/26.
//

#ifndef CSC_481_BIGINT_BIGINT_DECLARATION_HPP
#define CSC_481_BIGINT_BIGINT_DECLARATION_HPP


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
#include <string>

/// @brief The underlying type used to store digits in a bigint
/// @see BigInt
typedef std::uint64_t limb_t;

/// @brief Maximum allowed digits to store inside one limb
/// @see limb_t
constexpr std::size_t LIMB_DIGITS = std::numeric_limits<limb_t>::digits10 - 1;

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
	inline void setToZero(const bool shrink = false);

	/// @brief Returns the limb count of the bigint
	[[nodiscard]] inline std::size_t getLimbCount() const noexcept;
	
	/// @brief Returns the total current digit count of the bigint
	/// @see BigInt::getLimbCount()
	[[nodiscard]] inline std::uintmax_t getDigitCount() const noexcept;
	
	/// @brief Returns the limb value at the given index, 0-based
	/// @note If the index is outside of the bigint's range, returns a 0
	[[nodiscard]] inline limb_t getLimb(const std::size_t index) const noexcept;
	
	/// @brief Returns the last, **Most** Significant, limb of the Bigint
	[[nodiscard]] inline limb_t getLastLimb() const noexcept;
	
	/// @brief Returns the first, **Least** Significant, limb of the Bigint
	[[nodiscard]] inline limb_t getFirstLimb() const noexcept;
	
	/// @brief Returns true if bigint is negative, false otherwise
	[[nodiscard]] inline bool isNegative() const noexcept;
	
	/// @brief Returns true if bigint evaluates to 0, false otherwise
	/// @note It's evaluated to 0 only if it contains only 1 limb, and that limb is equal to 0
	/// @see BigInt::getLimbCount()
	/// @see BigInt::getFirstLimb()
	[[nodiscard]] inline bool isZero() const noexcept;
	
	/// @brief Returns the current bigint as string representation, in decimal notation
	/// @note The length of the string is at most 1 * BigInt::isNegative() + BigInt::getDigitCount()
	/// @see BigInt::isNegative()
	/// @see BigInt::getDigitCount()
	[[nodiscard]] std::string toStringDec() const;
	
	/// @brief Sets the negative flag of the bigint to the passed parameter
	/// @param neg The negativity status value
	/// @see BigInt::isNegative()
	inline void setNegative(const bool neg);
	
	/// @brief Sets the limb, at the specified index, to the specified value
	/// @param index Index of the limb (0-based) at which apply the provided value
	/// @param value Value of the limb to be set at the passed index
	/// @note If passed value has more digits than STORAGE_MAX, returns false without modifying anything
	/// @note Setting the last limb to 0 deletes that last limb. If the last limb is also the first limb, it sets the bigint to 0 instead
	/// @note If the index is outside of the range, it allocates as many limbs as needed to fit that index
	/// @returns True if limb setting was successful, falsed otherwise
	/// @see STORAGE_MAX
	/// @see BigInt::setToZero()
	bool setLimb(const std::size_t index, const limb_t value);
	
	/// @brief Removes the leading 0-limbs in front of the number, and removes negativity if number evaluates to 0
	/// @note Helps to remove confusion from algorithms that expect the most significant limb to be non-zero. 
	/// @note Remember to run this after each non-normalising operation that reduces the bigint size.
	/// @see BigInt::isZero()
	void normalise() {
		while (this->getLimbCount() > 1 && this->getLastLimb() == 0) {
			this->m_lstDigits.pop_back();
		}
		
		if (this->isZero()) {
			this->setNegative(false);
		}
	}
	
private:
	
	/// @brief Completely clear the underlying bigint: clear containers and remove negativity
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	inline void clear(const bool shrink = false);

	/// Storage for the limbs
	/// @see limb_t
	std::deque<limb_t> m_lstDigits;
	
	/// Flag to indicate whether the bigint is negative
	bool m_bNegative;
};



#endif //CSC_481_BIGINT_BIGINT_DECLARATION_HPP
