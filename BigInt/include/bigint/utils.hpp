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

// if debugmode wasn't defiend prior to inclusion of the utils, subtitute our own, and allow debug mode.
// Will probably change it later
#ifndef DEBUGMODE
/// @brief Macro to toggle the debug functionality in the library
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

template <std::integral T>
requires(std::numeric_limits<T>::is_specialized) 
[[nodiscard]] constexpr std::size_t digitCount(T val) noexcept {
	
	if constexpr (std::is_same<std::remove_cvref<T>, bool>::value) {
		return 1; // bool is always 1 digit (why is it integral even)
	}
	else {
		if constexpr (std::is_signed<T>::value) {
			if (val == std::numeric_limits<T>::lowest()) {
				return std::numeric_limits<T>::digits10 + 1;
			}
			if (val < 0) {
				val *= -1;
			}
		}
	
		T temp = 1;
		for (std::size_t i = 0; i < std::numeric_limits<T>::digits10; i++) {
			temp *= 10;
			if (val < temp) {
				return i+1;
			}
		}
	
		return std::numeric_limits<T>::digits10 + 1;
	}
	
	
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
