//
// Created by artyk on 10/30/26.
//

#ifndef CSC_481_BIGINT_UTILS_HPP
#define CSC_481_BIGINT_UTILS_HPP

#include <cstdio>
#include <iostream>
#include <concepts>
#include <cinttypes>
#include <cmath>
#include <limits>
#include <type_traits>
#include <string_view>
#include <cstddef>

// if debugmode wasn't defiend prior to inclusion of the utils, subtitute our own, and allow debug mode.
// Will probably change it later
#ifndef DEBUGMODE
/// @brief Macro to toggle the debug functionality in the library
/// @TODO Change debug mode to be off by default
/// That includes debug printing.
#define DEBUGMODE 1
#endif

/// @brief Macro for quick typing of the new character symbol
#define NLC '\n'

/// @brief Macro for quick typing of the new character c-string
#define NLS "\n"

/// @brief Macro for the check **(dividend % divisor) == 0**.
/// @param dividend The dividend of the operation
/// @param divisor The divisor of the operation
#define mod0(dividend, divisor) (!(dividend % divisor))

constexpr std::string_view boolToString(const bool val) noexcept {
	return ( val ) ? "true" : "false";
}

template <typename T>
	requires (
		std::numeric_limits<std::remove_cv_t<T>>::is_specialized &&
		std::numeric_limits<std::remove_cv_t<T>>::is_integer &&
		std::numeric_limits<std::remove_cv_t<T>>::is_bounded
	)
[[nodiscard]] constexpr std::size_t digitCount(T value) noexcept {
	using V = std::remove_cv_t<T>;
	using Limits = std::numeric_limits<V>;

	if constexpr (std::same_as<V, bool>) {
		return 1;
	}
	else {
		V x = value;

		// Normalize signed values to negative.
		if constexpr (Limits::is_signed) {
			if (x > V{0}) {
				x = -x;
			}
		}

		// Determine the maximum decimal digit count
		// entirely at compile time.
		constexpr std::size_t maxDigits = [] {
			V bound = [] {
				if constexpr (Limits::is_signed) {
					return Limits::lowest();
				}
				else {
					return Limits::max();
				}
			}();

			std::size_t n = 1;

			while (( bound /= 10 ) != 0) {
				++n;
			}

			return n;
		}();

		// Generate a binary search tree at compile time.
		const auto search =
			[&]<std::size_t lo, std::size_t hi>(
			auto&& self
		) -> std::size_t {
			if constexpr (lo == hi) {
				return lo;
			}
			else {
				constexpr std::size_t mid =
					lo + ( hi - lo + 1 ) / 2;

				// Generate the threshold at compile time.
				constexpr V threshold = [](std::size_t digits) {
					V power = V{1};

					if constexpr (Limits::is_signed) {
						power = -power;
					}

					for (std::size_t i = 1; i < digits; ++i) {
						power = static_cast<V>(power * 10);
					}

					return power;
				}(mid);

				if constexpr (Limits::is_signed) {
					return x <= threshold
						       ? self.template operator()<mid, hi>(self)
						       : self.template operator()<lo, mid - 1>(self);
				}
				else {
					return x >= threshold
						       ? self.template operator()<mid, hi>(self)
						       : self.template operator()<lo, mid - 1>(self);
				}
			}
		};

		return search.template operator()<1, maxDigits>(search);
	}
}


template <bool inclusive = true, typename T, typename Y>
constexpr bool isInRange(const T& low, const T& high, const Y& val) {
	if constexpr (inclusive) {
		if (val < low || val > high) {
			return false;
		}
	}
	else {
		if (val <= low || val >= high) {
			return false;
		}
	}
	return true;
}

constexpr bool charIsNum(const char val) noexcept {
	return isInRange<true, char, char>('0', '9', val);
}

constexpr std::string_view trimWhitespace(const std::string_view str) noexcept {
	constexpr std::string_view whitespace = " \t\n\r\f\v";

	const auto first = str.find_first_not_of(whitespace);
	if (first == std::string_view::npos)
		return {};

	const auto last = str.find_last_not_of(whitespace);
	return str.substr(first, last - first + 1);
}


/// @brief Struct to contain the results of the string validation performed by the function
struct StrValidation {
	/// @brief Enum to communicate the status of the validated string
	enum class ValStatus : int {
		/// String represents a valid number, conforms to all rules
		Valid,
		/// String is empty or full or whitespace characters
		Empty,
		/// String doesn't represent a valid number, some rules failed
		Invalid,
	};

	/// @brief Integer part of the number
	/// @remark Sign is excluded
	/// @note It's empty if string failed the validation
	std::string_view intPart;
	/// @brief Decimal fraction part of the number if provided
	/// @remark Sign and the decimal dot are excluded
	/// @note It's empty if string failed the validation, or no decimal fraction was detected
	std::string_view decPart;

	/// @brief The result/status of the validation
	ValStatus status = ValStatus::Invalid;

	/// @brief Flag to indicate whether the parsed number is negative
	bool negative = false;
	/// @brief Flag to indicate whether the decimal fraction part is non-zero
	bool hasNonZeroFraction = false;
};

inline StrValidation strIsNum(std::string_view str) noexcept {
	StrValidation result;

	str = trimWhitespace(str);
	
	// empty string, for some reason
	if (str.empty()) {
		return {.status = StrValidation::ValStatus::Empty};
	}
	
	//assume that the string is at least 1 non-whitespace char 
	std::size_t i = 0;
	
	// sign work
	bool metPeriod = false;
	
	switch (str[i]) {
		
	case '-': 
		result.negative = true;
		[[fallthrough]];
	case '+':
		if (str.size() == 1) {
			return {.status = StrValidation::ValStatus::Invalid};
		}
		str.remove_prefix(1); // move the starting char to the next, if it's a sign
		break;
		
	default:
		
	}
	
	//integer part
	for (; i < str.size(); i++) {
		if (!charIsNum(str[i])) {
			if (str[i] == '.') {
				metPeriod = true;
				break;
			}
			return {.status = StrValidation::ValStatus::Invalid};
		}
	}
	
	result.intPart = str.substr(0, i);
	if (metPeriod) {
		result.decPart = str.substr(++i);
	}
	
	//decimal part
	for (; i < str.size(); i++) {
		if (!charIsNum(str[i])) {
			return {.status = StrValidation::ValStatus::Invalid};
		}
		if (!result.hasNonZeroFraction) {
			if (str[i] > '0') {
				// literally if larger than 0 (now constrained to 0->9)
				result.hasNonZeroFraction = true;
			}
		}
	}
	
	// check if we even scanned any actual digits
	if (result.intPart.empty() && result.decPart.empty()) {
		return {.status = StrValidation::ValStatus::Invalid};
	}
	
	result.status = StrValidation::ValStatus::Valid;
	
	return result;
}


#if DEBUGMODE
/// @brief Macro for the **debug printf()** printing, use the same way as you have used printf before.
/// Prints "[DEBUG] [filename] [function name that called it::invoked line] -> stuff that you wanted the printf to print"
/// @note Prints additional newline with each print
/// @note Only works in debug mode. Does nothing if #DEBUGMODE is not set to 1
/// @see #DEBUGMODE
#define dprintf(...) std::printf("[DEBUG] [%s] [%s()::%llu] -> ", __FILE__, __func__, static_cast<unsigned long long>(__LINE__)); \
	std::printf(__VA_ARGS__); \
	std::printf("\n");

/// @brief Macro for the **debug cout** printing, use the same way as you have used cout for simple printing before.
/// Prints "[DEBUG] [filename] [function name that called it::invoked line] -> stuff that you wanted the printf to print"
/// @note Use it without the `std` namespace before it
/// @note Only works in debug mode. Does nothing if #DEBUGMODE is not set to 1
/// @see #DEBUGMODE
#define cdbg std::cout << "[DEBUG] [" __FILE__ "] [" <<__func__<<"()::" << __LINE__ <<"] -> "
#else
#define dprintf(...)

#define cdbg if constexpr(false) std::cout
#endif // DEBUGMODE 


#endif //CSC_481_BIGINT_UTILS_HPP
