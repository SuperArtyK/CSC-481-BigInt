//
// Created by artyk on 11/21/26.
//

#ifndef CSC_481_BIGINT_BIGINT_CONFIG_HPP
#define CSC_481_BIGINT_BIGINT_CONFIG_HPP

#ifndef DEBUGMODE
/// @brief Enables or disables BigInt diagnostic output.
/// @note May be overridden by a compiler definition.
/// @TODO Change debug mode to be off by default
#define DEBUGMODE 1
#endif

#ifdef CSC_481_BIGINT_UTILS_HPP
#error Utils.hpp was included before bigint_config.hpp! Some default settings there overshadow settings here, so make sure ot fix the include order
#endif

#endif //CSC_481_BIGINT_BIGINT_CONFIG_HPP
