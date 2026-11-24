//
// Created by artyk on 10/31/26.
//

#ifndef CSC_481_BIGINT_BIGINT_DECLARATION_HPP
#define CSC_481_BIGINT_BIGINT_DECLARATION_HPP

#include "bigint_config.hpp"
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
	10000000000000000000ull
};

/// @brief The base-10 mask for the maximum digits that can be stored in limb
/// @note Usage example: value % STORAGE_MASK -> trimmed value that can fit in the limb
/// @see limb_t
constexpr limb_t STORAGE_MASK = powersOf10[LIMB_DIGITS];

/// @brief The maximum value that the limb can store
/// @see limb_t
/// @see STORAGE_MASK
constexpr limb_t STORAGE_MAX = STORAGE_MASK - 1;

/// @brief The maximum amount of limbs whose digits can be calculated precisely
/// @see LIMB_DIGITS
constexpr std::uintmax_t LIMB_DIGITCOUNT_MAX = std::numeric_limits<std::uintmax_t>::max() / LIMB_DIGITS;

namespace bigint_test {
	struct Inspector;
}


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

	/// @brief Construct the bigint from the passed integer
	/// @tparam T Any integer type, according to std::integral
	/// @param num The value of the passed integer
	template <std::integral T>
	BigInt(const T num);

	/// @brief Construct the bigint from the passed floating point
	/// @tparam T Any float type, according to std::floating_point
	/// @param num The value of the passed float
	/// @throw std::domain_error If the passed float is not finite
	template <std::floating_point T>
	BigInt(const T num);

	/// @brief Constructs a BigInt from decimal numeric text.
	/// @param str Decimal text to convert.
	/// @throws std::invalid_argument If the nonempty text is malformed.
	/// @note Fractional digits are truncated toward zero.
	/// @note Empty or space/tab-only input produces canonical zero.
	/// @note Successful conversion produces a normalized representation.
	/// @see BigInt::copyFromString()
	explicit BigInt(const std::string_view str);

	/// @brief Copy-assignment for the class
	/// @param bint the bigint to copy values from
	/// @return Reference to the object after the assignment
	BigInt& operator=(const BigInt& bint);

	/// @brief Move-assignment for the class
	/// @param bint the bigint to move values from
	/// @return Reference to the object after the assignment
	BigInt& operator=(BigInt&& bint) noexcept;

	/// @brief Assign the bigint to the passed integer value
	/// @tparam T Any integer type, according to std::integral
	/// @param num The value of the passed integer
	/// @return Reference to the object after the assignment
	template <std::integral T>
	BigInt& operator=(const T num);

	/// @brief Assign the bigint to the passed floating point value
	/// @tparam T Any float type, according to std::floating_point
	/// @param num The value of the passed float
	/// @throw std::domain_error If the passed float is not finite
	/// @return Reference to the object after the assignment
	template <std::floating_point T>
	BigInt& operator=(const T num);

	/// @brief Assigns the value of the bigint to the passed string, if it's a valid number
	/// @param str The string to copy the numbers from, after validating
	/// @throw std::invalid_argument If the string is not a valid number
	/// @note Invalid input preserves the previous stored representation.
	/// @return Reference to the object after the assignment
	/// @see BigInt::copyFromString()
	BigInt& operator=(const std::string_view str);

	/// @brief Set the underlying bigint to 0
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	inline void setToZero(const bool shrink = false);

	/// @brief Returns the limb count of the bigint
	/// @return limb_t value of current total limb count
	[[nodiscard]] inline std::size_t getLimbCount() const noexcept;

	/// @brief Retrieves the total current digit count of the bigint
	/// @pre The BigInt is normalized.
	/// @attention If limbcount exceeds LIMB_DIGITCOUNT_MAX (overflows), it returns 0
	/// @pre The BigInt is normalized.
	/// @see BigInt::getLimbCount()
	/// @return Calculated total digit count as limb_t
	[[nodiscard]] inline std::uintmax_t getDigitCount() const noexcept;

	/// @brief Retrieves the limb value at the given index, 0-based
	/// @param index Zero-based limb index, least significant first.
	/// @note If the index is outside of the bigint's range, returns a 0
	/// @return limb_t value of the limb at the specified index
	[[nodiscard]] inline limb_t getLimb(const std::size_t index) const noexcept;

	/// @brief Retrieves the last, **Most** Significant, limb of the Bigint
	/// @pre The BigInt contains at least one limb.
	/// @warning Calling this function with empty storage is undefined behavior.
	/// @return limb_t value of the last limb
	[[nodiscard]] inline limb_t getLastLimb() const noexcept;

	/// @brief Returns the first, **Least** Significant, limb of the Bigint
	/// @pre The BigInt contains at least one limb.
	/// @warning Calling this function with empty storage is undefined behavior.
	/// @return limb_t value of the first limb
	[[nodiscard]] inline limb_t getFirstLimb() const noexcept;

	/// @brief Returns true if bigint is negative, false otherwise
	/// @return bool value of the negativity status
	[[nodiscard]] inline bool isNegative() const noexcept;

	/// @brief Checks whether the bigint has magnitude of 0
	/// @pre The BigInt is normalized.
	/// @note It's evaluated to 0 only if it contains only 1 limb, and that limb is equal to 0
	/// @see BigInt::getLimbCount()
	/// @see BigInt::getFirstLimb()
	/// @return True if magnitude is 0, false otherwise
	[[nodiscard]] inline bool isZero() const noexcept;


	/// @brief Returns the complete decimal representation of the BigInt.
	/// @pre The BigInt is normalized.
	/// @note Zero is represented as "0".
	/// @note The length of the string is at most 1 * BigInt::isNegative() + BigInt::getDigitCount()
	/// @return Exact decimal text without rounding or scientific notation.
	[[nodiscard]] std::string toStringDec() const;

	/// @brief Sets the negative flag of the bigint to the passed parameter
	/// @param neg The negativity status value
	/// @see BigInt::isNegative()
	inline void setNegative(const bool neg);

	/// @brief Sets a magnitude limb and normalizes the result.
	/// @param index Zero-based limb index, least significant first.
	/// @param value New limb value.
	/// @return True on success; false if the value exceeds STORAGE_MAX, without modifying the object.
	/// @note An out-of-range zero write is a successful no-op, without resizing or normalization.
	/// @note Successful writes preserve the sign unless the resulting magnitude is zero.
	/// @warning Storage allocation and capacity failures may throw exceptions.
	/// @throws std::length_error If index equals **std::numeric_limits<std::size_t>::max()**
	/// @see BigInt::setLimbRaw()
	/// @see BigInt::normalise()
	bool setLimb(const std::size_t index, const limb_t value);

	/// @brief Sets a magnitude limb without normalizing the representation.
	/// @param index Zero-based limb index, least significant first.
	/// @param value New limb value.
	/// @return True on success; false if the value exceeds STORAGE_MAX, without modifying the object.
	/// @note Out-of-range writes extend storage with zero-filled limbs, even when value is zero.
	/// @attention The resulting representation may require normalization before numerical operations.
	/// @warning Storage allocation and capacity failures may throw exceptions.
	/// @throws std::length_error If index equals **std::numeric_limits<std::size_t>::max()**
	/// @see BigInt::setLimb()
	/// @see BigInt::normalise()
	bool setLimbRaw(const std::size_t index, const limb_t value);

	/// @brief Removes the most significant 0-limbs of the number, and removes negativity if number evaluates to 0
	/// @note Helps to remove confusion from algorithms that expect the most significant limb to be non-zero. 
	/// @note Remember to run this after each non-normalising operation that reduces the bigint size.
	/// @see BigInt::isZero()
	void normalise();

	/// @brief Checks whether bigint is normalised
	/// @note By normalised it means: at least 1 limb, without leading 0's; and if it is zero, then it is not negative
	/// @warning Most functions assume that the bigint is normalised!
	/// @returns True if bigint is normalised, false otherwise
	/// @see BigInt::isZero()
	[[nodiscard]] bool isNormalised() const noexcept;

	explicit operator bool() const noexcept {
		return this->isZero();
	}

	[[nodiscard]] std::string toStringSci(const std::size_t maxSigDig = 20, const bool pad = false) const;

private:
	/// @brief Set the bigint to the passed integer value
	/// @tparam T Any integer type, according to std::integral
	/// @param num The value of the passed integer
	template <std::integral T>
	void copyFromInt(const T num);

	/// @brief Replaces the value with the integer part of a floating-point number.
	/// @tparam T Floating-point type supported by std::floating_point.
	/// @param num Floating-point value to convert.
	/// @throws std::domain_error If the value is NaN or infinity.
	/// @note Truncates toward zero, preserving the exact integer part of the received floating-point value.
	/// @note Successful conversion produces a normalized representation.
	template <std::floating_point T>
	void copyFromFloat(const T num);

	/// @brief Sets the value of the bigint to the passed string, if it's a valid number
	/// @param str The string to copy the numbers from, after validating
	/// @throw std::invalid_argument If the string is not a valid number
	/// @see strIsNum()
	void copyFromString(const std::string_view str);

	/// @brief Replaces the value using prevalidated decimal components.
	/// @param result Validated numeric components or an Empty validation result.
	/// @pre The validation status is Valid or Empty.
	/// @note The fractional part is discarded, and the result is normalized.
	/// @note Empty input or a zero integer portion produces canonical zero.
	void copyFromValidString(const StrValidation& result);

	/// @brief Completely clear the underlying bigint: clear containers and remove negativity
	/// @param shrink Flag whether to shrink the underlying containers to fit (reallocate to size)
	inline void clear(const bool shrink = false);

	/// Storage for the limbs
	/// @see limb_t
	std::deque<limb_t> m_lstDigits;

	/// Flag to indicate whether the bigint is negative
	bool m_bNegative;

	//friendzone :3
	friend bigint_test::Inspector;
};


#endif //CSC_481_BIGINT_BIGINT_DECLARATION_HPP
