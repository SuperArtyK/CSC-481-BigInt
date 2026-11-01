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
#include <sys/stat.h>

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
	inline void setToZero(const bool shrink);


	[[nodiscard]] inline std::size_t getLimbCount() const noexcept;

	[[nodiscard]] inline std::uintmax_t getDigitCount() const noexcept;

	[[nodiscard]] inline limb_t getLimb(const std::size_t index) const noexcept;

	[[nodiscard]] inline limb_t getLastLimb() const noexcept;

	[[nodiscard]] inline limb_t getFirstLimb() const noexcept;

	[[nodiscard]] inline bool isNegative() const noexcept;

	[[nodiscard]] inline bool isZero() const noexcept;

	inline void setNegative(const bool neg);
	
	bool setLimb(const std::size_t index, const limb_t value);

	std::string toStringDec() const;

private:
	
	/// @brief Completely clear the underlying bigint: clear containers and remove negativity
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	inline void clear(const bool shrink);

	/// Storage for the limbs
	/// @see limb_t
	std::deque<limb_t> m_lstDigits;
	
	/// Flag to indicate whether the bigint is negative
	bool m_bNegative;
};



#endif //CSC_481_BIGINT_BIGINT_DECLARATION_HPP
