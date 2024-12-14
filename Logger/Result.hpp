/**
 * @file Result.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief My implementation of std::expected
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

// NOLINTBEGIN

#ifndef MLIB_RESULT_HPP
#define MLIB_RESULT_HPP

#include <new> // IWYU pragma: keep
#include <utility>

#include "Exception.hpp"

namespace mlib {
namespace err {

/** @struct Result
 * @brief Struct for returning errors with values
 *
 * @tparam T type of value
 */
template<typename T>
class Result
{
public:
    /**
     * @brief Construct an error Result
     *
     * @param error
     */
    Result(ErrorCode error, detail::SourcePosition pos = {}) noexcept
        : m_error(error, pos), m_ok(false)
    {
        new(&m_error) Exception{error, pos};
    }

    /**
     * @brief Construct a valid Result
     *
     * @tparam U perfect forwarding
     * @param value
     */
    template<typename U = T>
    Result(U&& value)
        : m_ok(true)
    {
        new(&m_value) T{std::forward<U>(value)};
    }

    [[nodiscard]] operator bool() const noexcept { return IsValue(); }
    [[nodiscard]] operator T() = delete;

    /**
     * @brief Check if it has value
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool IsValue() const noexcept { return m_ok; }

    /**
     * @brief Check if it is error
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool IsError() const noexcept { return !m_ok; }

    /**
     * @brief Get value. UB if error
     *
     * @return T&
     */
    [[nodiscard]] T& operator*() noexcept
    {
        return m_value;
    }

    /**
     * @brief Access value members. UB if error
     *
     * @return T*
     */
    [[nodiscard]] T* operator->() noexcept
    {
        return &m_value;
    }

    /**
     * @brief Get value. Throws error code if error
     *
     * @return T&
     */
    [[nodiscard]] T& Value()
    {
        if (IsError())
        {
            throw m_error;
        }

        return m_value;
    }

    /**
     * @brief Returns value of defaultValue if value is not present
     *
     * @param [in] defaultValue
     * @return T
     */
    template<typename U = T>
    [[nodiscard]] T ValueOr(U&& defaultValue) const
    {
        if (IsValue())
            return m_value;
        return std::forward<U>(defaultValue);
    }

    /**
     * @brief Return error if it is error
     *
     * @return ErrorCode
     */
    [[nodiscard]] ErrorCode Error() const noexcept
    {
        if (IsValue())
            return EVERYTHING_FINE;
        return m_error.GetError();
    }

    /**
     * @brief Destroy the Result
     */
    ~Result()
    {
        if (IsValue())
            m_value.~T();
    }
private:
    union
    {
        T m_value;
        Exception m_error;
    };

    bool m_ok;
};

} // namespace err
} // namespace mlib

#endif // MLIB_RESULT_HPP

// NOLINTEND
