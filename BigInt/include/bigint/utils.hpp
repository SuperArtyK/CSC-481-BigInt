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
    } else {
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
                } else {
                    return Limits::max();
                }
            }();

            std::size_t n = 1;

            while ((bound /= 10) != 0) {
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
                } else {
                    constexpr std::size_t mid =
                        lo + (hi - lo + 1) / 2;

                    // Generate the threshold at compile time.
                    constexpr V threshold = [] {
                        V power = V{1};

                        if constexpr (Limits::is_signed) {
                            power = -power;
                        }

                        for (std::size_t i = 1; i < mid; ++i) {
                            power = static_cast<V>(power * 10);
                        }

                        return power;
                    }();

                    if constexpr (Limits::is_signed) {
                        return x <= threshold
                            ? self.template operator()<mid, hi>(self)
                            : self.template operator()<lo, mid - 1>(self);
                    } else {
                        return x >= threshold
                            ? self.template operator()<mid, hi>(self)
                            : self.template operator()<lo, mid - 1>(self);
                    }
                }
            };

        return search.template operator()<1, maxDigits>(search);
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
