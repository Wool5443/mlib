/**
 * @file ConsoleColor.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Console coloring
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 */

// NOLINTBEGIN

#ifndef MLIB_LOGGER_CONSOLE_COLOR_HPP
#define MLIB_LOGGER_CONSOLE_COLOR_HPP

#include <cstdio>

#ifdef __linux
#include <unistd.h>
#endif

namespace mlib {
namespace detail {

/** @enum ConsoleColor
 * @brief Represents colors for @see SetConsoleColor
 */
enum class ConsoleColor
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
};

/**
 * @brief Checks if file supports colors
 *
 * @param [in] file
 *
 * @return true
 * @return false
 */
static inline bool SupportsColors(FILE* file) noexcept
{
#ifdef __linux
    return isatty(fileno(file));
#endif
    return false;
}

/**
 * @brief Set the console color
 *
 * @param [in] out stream
 * @param [in] color
 */
static inline void SetConsoleColor(FILE* file, ConsoleColor color)
{
    if (SupportsColors(file))
        fprintf(file, "\033[0;%dm", static_cast<int>(color));
}

} // namespace detail
} // namespace mlib

#endif // MLIB_LOGGER_CONSOLE_COLOR_HPP

// NOLINTEND
