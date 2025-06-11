/**
 * @file ConsoleColor.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Utility functions for setting console text colors.
 *
 * This header provides utilities to set the console text color using ANSI escape codes.
 * It supports checking if the terminal can display colors and allows the setting of various
 * predefined colors for console output.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 */

#ifndef MLIB_LOGGER_CONSOLE_COLOR_HPP
#define MLIB_LOGGER_CONSOLE_COLOR_HPP

#include <cstdio>

#ifdef __linux
#include <unistd.h>
#endif

namespace mlib {
namespace detail {

/**
 * @enum Console_color
 * @brief Represents the colors available for console output.
 *
 * This enum defines various color options for console output using ANSI escape codes.
 * These values can be passed to the `set_console_color` function to change the color of the console text.
 */
enum class Console_color
{
    BLACK = 30, ///< Black color
    RED,        ///< Red color
    GREEN,      ///< Green color
    YELLOW,     ///< Yellow color
    BLUE,       ///< Blue color
    MAGENTA,    ///< Magenta color
    CYAN,       ///< Cyan color
    WHITE,      ///< White color
};

/**
 * @brief Checks if the terminal supports color output.
 *
 * This function checks if the given output stream (typically a console or terminal) supports
 * color output, based on the platform and whether the terminal supports ANSI escape codes.
 *
 * @param [in] file The output stream (typically `stdout`, `stderr`, or a file stream).
 *
 * @return `true` if the terminal supports colors, `false` otherwise.
 */
inline bool supports_colors(FILE* file) noexcept
{
#ifdef __linux
    // On Linux, check if the terminal supports colors using the isatty function
    return isatty(fileno(file));
#endif
    // By default, assume that color is not supported on non-Linux platforms
    return false;
}

/**
 * @brief Sets the color for console output.
 *
 * This function sets the color of the console text by writing the appropriate ANSI escape
 * sequence to the output stream. The color is set only if the output stream supports color.
 *
 * @param [in] file The output stream to change the color for (typically `stdout` or `stderr`).
 * @param [in] color The color to set, represented by a `Console_color` value.
 */
inline void set_console_color(FILE* file, Console_color color)
{
    // Only set the color if the terminal supports it
    if (supports_colors(file))
        // Print the ANSI escape sequence for the given color
        fprintf(file, "\033[0;%dm", static_cast<int>(color));
}

} // namespace detail
} // namespace mlib

#endif // MLIB_LOGGER_CONSOLE_COLOR_HPP
