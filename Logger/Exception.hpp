/**
 * @file Exception.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Exception class that stores where it happened
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MLIB_LOGGER_EXCEPTION_HPP
#define MLIB_LOGGER_EXCEPTION_HPP

#include "details/Source_position.hpp"
#include "details/Error_code.hpp"

namespace mlib {
namespace err {

class Exception : public detail::Source_position
{
public:
    explicit Exception(Error_code error_code, Source_position position = {}) noexcept
        : Source_position(position), m_error_code(error_code) {}

    [[nodiscard]] operator bool() const noexcept { return m_error_code != Error_code::EVERYTHING_FINE; }

    [[nodiscard]] constexpr Error_code get_error() const noexcept { return m_error_code; }
private:
    Error_code m_error_code;
};

} // namespace err
} // namespace mlib

#define MLIB_MAKE_EXCEPTION(error_code) err::Exception(error_code, CURRENT_SOURCE_POSITION())

#endif // MLIB_LOGGER_EXCEPTION_HPP
