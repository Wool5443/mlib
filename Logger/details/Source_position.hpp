/**
 * @file SourcePosition.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Source position class for capturing file, function, and line information.
 *
 * This header defines the `Source_position` class, which encapsulates information about the
 * source code location, such as the file name, function name, and line number. This is typically
 * used for logging or debugging purposes to track where specific events or errors occur in the code.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MLIB_LOGGER_SOURCE_POSITION_HPP
#define MLIB_LOGGER_SOURCE_POSITION_HPP

#include <cstddef>

namespace mlib {
namespace detail {

/**
 * @brief Class representing the position of the source code in a file.
 *
 * The `Source_position` class stores the file name, function name, and line number, which
 * can be used for logging or debugging to identify the exact location of an event in the code.
 * This class helps capture the source code location automatically through macros.
 */
class Source_position {
public:
    /**
     * @brief Default destructor for `Source_position`.
     *
     * The destructor is virtual to allow for safe deletion through a base class pointer.
     */
    virtual ~Source_position() = default;

    /**
     * @brief Default constructor for `Source_position`.
     *
     * This constructor initializes an empty source position where the file name, function name,
     * and line number are set to null or zero.
     */
    Source_position() = default;

    /**
     * @brief Constructs a `Source_position` object with the given file name, function name, and line number.
     *
     * This constructor allows the creation of a `Source_position` object with the source file,
     * function name, and line number. It is typically used to capture the position of an event
     * in the code, such as when logging an error or event.
     *
     * @param file_name The name of the source file where the event occurred.
     * @param func_name The name of the function where the event occurred.
     * @param line The line number where the event occurred.
     */
    explicit Source_position(const char* file_name, const char* func_name, size_t line) noexcept
        : m_file_name(file_name), m_func_name(func_name), m_line(line) {}

    /**
     * @brief Retrieves the file name where the event occurred.
     *
     * This function returns the file name as a string, which was captured when the `Source_position`
     * object was created.
     *
     * @return The name of the file where the event occurred.
     */
    const char* get_file_name() const noexcept { return m_file_name; }

    /**
     * @brief Retrieves the function name where the event occurred.
     *
     * This function returns the name of the function where the event occurred, captured when
     * the `Source_position` object was created.
     *
     * @return The name of the function where the event occurred.
     */
    const char* get_function_name() const noexcept { return m_func_name; }

    /**
     * @brief Retrieves the line number where the event occurred.
     *
     * This function returns the line number where the event occurred, captured when the
     * `Source_position` object was created.
     *
     * @return The line number where the event occurred.
     */
    size_t get_line() const noexcept { return m_line; }

private:
    const char* m_file_name = nullptr; ///< The file name where the event occurred.
    const char* m_func_name = nullptr; ///< The function name where the event occurred.
    size_t m_line = 0; ///< The line number where the event occurred.
};

/**
 * @brief Macro to get the current file name.
 *
 * This macro expands to the current file name at the point of invocation, typically used in
 * logging or debugging to capture the source file.
 */
#define GET_FILE_NAME() __FILE__

/**
 * @brief Macro to get the current line number.
 *
 * This macro expands to the current line number in the source code at the point of invocation,
 * typically used to capture the line number of an event.
 */
#define GET_LINE() __LINE__

/**
 * @brief Macro to get the current function name.
 *
 * This macro expands to the name of the current function at the point of invocation. It works
 * across different compilers, adjusting to either `__PRETTY_FUNCTION__` or `__func__` depending
 * on the compiler.
 */
#if defined(__clang__) || defined(__GNUC__)
#define GET_FUNCTION_NAME()  __PRETTY_FUNCTION__
#else
#define GET_FUNCTION_NAME() __func__
#endif

/**
 * @brief Macro to create a `Source_position` object representing the current source location.
 *
 * This macro creates a `Source_position` object that encapsulates the current file name,
 * function name, and line number. It is typically used for logging or debugging purposes
 * to capture the exact source location.
 */
#define CURRENT_SOURCE_POSITION() \
mlib::detail::Source_position(GET_FILE_NAME(), GET_FUNCTION_NAME(), GET_LINE())

} // namespace detail
} // namespace mlib

#endif // MLIB_LOGGER_SOURCE_POSITION_HPP
