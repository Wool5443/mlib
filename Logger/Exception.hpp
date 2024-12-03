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

// NOLINTBEGIN

#ifndef MLIB_LOGGER_EXCEPTION_HPP
#define MLIB_LOGGER_EXCEPTION_HPP

#include "include/SourcePosition.hpp"
#include "include/ErrorCode.hpp"

namespace mlib {
namespace err {

class Exception : public SourcePosition
{
public:
    explicit Exception(ErrorCode errorCode, SourcePosition position = {}) noexcept
        : SourcePosition(position), m_errorCode(errorCode) {}

    operator bool() const noexcept { return m_errorCode != ErrorCode::EVERYTHING_FINE; }

    constexpr ErrorCode GetError() const noexcept { return m_errorCode; }
private:
    ErrorCode m_errorCode;
};

} // namespace err
} // namespace mlib

#endif // MLIB_LOGGER_EXCEPTION_HPP

// NOLINTEND
