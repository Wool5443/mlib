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

#include "details/SourcePosition.hpp"
#include "details/ErrorCode.hpp"

namespace mlib {
namespace err {

class Exception : public detail::SourcePosition
{
public:
    explicit Exception(ErrorCode errorCode, SourcePosition position = {}) noexcept
        : SourcePosition(position), m_errorCode(errorCode) {}

    [[nodiscard]] operator bool() const noexcept { return m_errorCode != ErrorCode::EVERYTHING_FINE; }

    [[nodiscard]] constexpr ErrorCode GetError() const noexcept { return m_errorCode; }
private:
    ErrorCode m_errorCode;
};

} // namespace err
} // namespace mlib

#define MLIB_MAKE_EXCEPTION(errorCode) err::Exception(errorCode, CURRENT_SOURCE_POSITION())

#endif // MLIB_LOGGER_EXCEPTION_HPP
