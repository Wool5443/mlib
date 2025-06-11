/**
 * @file Exception.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Exception class that stores the location and error code of where it happened.
 *
 * This header defines the `Exception` class that extends the `Source_position` class to store
 * the error code and the source position (file, function, and line number) of where the exception occurred.
 * This class helps provide detailed information about errors, including where they occurred in the code.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MLIB_LOGGER_EXCEPTION_HPP
#define MLIB_LOGGER_EXCEPTION_HPP

#include "details/Source_position.hpp"
#include "details/Error_code.hpp"

namespace mlib {
namespace err {

/**
 * @class Exception
 * @brief A class that represents an exception, including the error code and source position.
 *
 * The `Exception` class extends `Source_position` to include information about where the exception
 * occurred in the source code (file, function, and line). Additionally, it stores an error code that
 * indicates the specific error that caused the exception. This class provides an operator to check
 * if the exception is valid based on the error code.
 */
class Exception : public detail::Source_position
{
public:
    /**
     * @brief Constructs an `Exception` object with the specified error code and optional source position.
     *
     * This constructor initializes the `Exception` with an error code and optionally a source position.
     * If the source position is not provided, it defaults to an empty position (i.e., no specific location).
     *
     * @param error_code The error code representing the specific error.
     * @param position The source position where the error occurred (defaults to an empty position).
     */
    explicit Exception(Error_code error_code, Source_position position = {}) noexcept
        : Source_position(position), m_error_code(error_code) {}

    /**
     * @brief Checks if the exception represents an error.
     *
     * This operator returns `true` if the exception has a valid error code (i.e., the error code is not
     * `Error_code::EVERYTHING_FINE`), indicating that an actual error has occurred.
     *
     * @return `true` if the error code is not `Error_code::EVERYTHING_FINE`, otherwise `false`.
     */
    [[nodiscard]] operator bool() const noexcept { return m_error_code != Error_code::EVERYTHING_FINE; }

    /**
     * @brief Retrieves the error code of the exception.
     *
     * This function returns the error code associated with the exception, which indicates the specific
     * error that occurred.
     *
     * @return The `Error_code` associated with the exception.
     */
    [[nodiscard]] constexpr Error_code get_error() const noexcept { return m_error_code; }

private:
    Error_code m_error_code; ///< The error code associated with the exception.
};

} // namespace err
} // namespace mlib

/**
 * @brief Macro for creating an `Exception` object with the current source position.
 *
 * This macro simplifies the creation of an `Exception` object by automatically capturing the current
 * source position (file, function, and line) and attaching it to the specified error code.
 *
 * @param error_code The error code for the exception.
 * @return An `Exception` object initialized with the error code and the current source position.
 */
#define MLIB_MAKE_EXCEPTION(error_code) err::Exception(error_code, CURRENT_SOURCE_POSITION())

#endif // MLIB_LOGGER_EXCEPTION_HPP
