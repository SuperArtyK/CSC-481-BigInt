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

/// @brief The underlying type used in the bigint's data storage
/// @see BigInt
typedef std::uint64_t limb_t;

/// @brief Maximum allowed digits to store inside one limb
/// @see limb_t
constexpr int limbDigits = std::numeric_limits<limb_t>::digits10 - 1;

/// @brief List of powers of 10 that would be used when working with the limb type.
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
};


/// @brief Big integer type class, copies the functionality of regular integers, but on arbitrary scale.
/// @note Technical limit for the size of the BigInt is std::size_t*digits(limb_t)
/// @see limb_t
/// @see limbDigits
class BigInt {
public:
	BigInt();

	BigInt(const BigInt& bint);

	BigInt(BigInt&& bint) noexcept;

	BigInt& operator=(const BigInt& bint);

	BigInt& operator=(BigInt&& bint) noexcept;

private:
	std::deque<limb_t> m_lstDigits;
	bool m_bNegative;
};


#endif //CSC_481_BIGINT_BIGINT_HPP
