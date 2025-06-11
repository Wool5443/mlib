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
     * @param [in] error
     *
     * @see mlib::err::Exception
     */
    Result(Error_code error, detail::Source_position pos = {}) noexcept
        : m_error{error, pos}, m_ok{true} {}

    /**
     * @brief Construct an error Result
     *
     * @param [in] error
     *
     * @see mlib::err::Exception
     */
    Result(Exception error) noexcept
        : m_error{error}, m_ok{true} {}

    /**
     * @brief Construct a valid Result
     *
     * @tparam U perfect forwarding
     * @param [in] value
     */
    template<typename U = T>
    Result(U&& value) noexcept(noexcept(T()))
        : m_value{std::forward<U>(value)}, m_ok{true} {}

    [[nodiscard]] operator bool() const noexcept { return is_value(); }
    [[nodiscard]] operator T() = delete;

    /**
     * @brief Check if it has value
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool is_value() const noexcept { return m_ok; }

    /**
     * @brief Check if it is error
     *
     * @return true
     * @return false
     */
    [[nodiscard]] bool is_error() const noexcept { return !m_ok; }

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
    [[nodiscard]] T& value()
    {
        if (is_error())
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
    [[nodiscard]] T value_or(U&& default_value) const
    {
        if (is_value())
            return m_value;
        return std::forward<U>(default_value);
    }

    /**
     * @brief Return error if it is error
     *
     * @return ErrorCode
     */
    [[nodiscard]] Error_code error() const noexcept
    {
        if (is_value())
        {
            return EVERYTHING_FINE;
        }
        return m_error.get_error();
    }

    /**
     * @brief Destroy the Result
     */
    ~Result()
    {
        if (is_value())
        {
            m_value.~T();
        }
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
