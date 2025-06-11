/**
 * @file Result.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Custom implementation of `std::expected`
 *
 * This header defines the `Result` class template, which is a custom implementation of the `std::expected`
 * type, used for representing either a value or an error. This structure is designed to be used in situations
 * where a function can return either a valid result or an error, providing a safe and convenient way to handle errors.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 */

#ifndef MLIB_RESULT_HPP
#define MLIB_RESULT_HPP

#include <new> // IWYU pragma: keep
#include <utility>

#include "Exception.hpp"

namespace mlib {
namespace err {

/**
 * @struct Result
 * @brief A class that holds either a value of type `T` or an error (`Exception`).
 *
 * The `Result` class is designed to represent a value that may be successfully computed or
 * an error that occurred during the computation. This pattern is useful in situations where
 * functions need to return either a result or an error without using exceptions.
 *
 * @tparam T The type of the value stored in the result.
 */
template<typename T>
class Result
{
public:
    /**
     * @brief Constructs an error `Result` with the specified error code.
     *
     * This constructor initializes the `Result` object to represent an error with a given
     * error code and an optional source position (for debugging purposes).
     *
     * @param error The error code representing the error.
     * @param pos The source position (file, function, line) where the error occurred (default is empty).
     *
     * @see mlib::err::Exception
     */
    Result(Error_code error, detail::Source_position pos = {}) noexcept
        : m_error{error, pos}, m_ok{false} {}

    /**
     * @brief Constructs an error `Result` from an existing `Exception` object.
     *
     * This constructor initializes the `Result` object with an `Exception` object, representing
     * an error. The `Exception` object contains both the error code and the source position.
     *
     * @param error The `Exception` object representing the error.
     *
     * @see mlib::err::Exception
     */
    Result(Exception error) noexcept
        : m_error{error}, m_ok{false} {}

    /**
     * @brief Constructs a valid `Result` that holds a value.
     *
     * This constructor initializes the `Result` object to represent a successful result containing
     * the value of type `T`. It uses perfect forwarding to efficiently construct the value.
     *
     * @tparam U The type of the value being forwarded.
     * @param value The value to store in the `Result`.
     */
    template<typename U = T>
    Result(U&& value) noexcept(noexcept(T()))
        : m_value{std::forward<U>(value)}, m_ok{true} {}

    /**
     * @brief Implicit conversion to `bool` to check if the `Result` contains a value.
     *
     * This operator returns `true` if the `Result` holds a valid value, and `false` if it contains an error.
     *
     * @return `true` if the `Result` contains a value, otherwise `false`.
     */
    operator bool() const noexcept { return is_value(); }

    /**
     * @brief Deleted conversion to `T` to prevent invalid access to value when `Result` is an error.
     */
    operator T() = delete;

    /**
     * @brief Checks if the `Result` contains a valid value.
     *
     * This function returns `true` if the `Result` holds a valid value of type `T`, and `false` if it holds an error.
     *
     * @return `true` if the `Result` contains a value, otherwise `false`.
     */
    [[nodiscard]] bool is_value() const noexcept { return m_ok; }

    /**
     * @brief Checks if the `Result` represents an error.
     *
     * This function returns `true` if the `Result` contains an error, and `false` if it contains a valid value.
     *
     * @return `true` if the `Result` is an error, otherwise `false`.
     */
    [[nodiscard]] bool is_error() const noexcept { return !m_ok; }

    /**
     * @brief Dereference operator to access the value stored in the `Result`.
     *
     * This operator returns a reference to the stored value, and it results in undefined behavior if the `Result`
     * contains an error.
     *
     * @return A reference to the value stored in the `Result`.
     */
    [[nodiscard]] T& operator*() noexcept
    {
        return m_value;
    }

    /**
     * @brief Member access operator to access the value stored in the `Result`.
     *
     * This operator returns a pointer to the stored value, and it results in undefined behavior if the `Result`
     * contains an error.
     *
     * @return A pointer to the value stored in the `Result`.
     */
    [[nodiscard]] T* operator->() noexcept
    {
        return &m_value;
    }

    /**
     * @brief Retrieves the value of the `Result`, throwing an exception if it is an error.
     *
     * This function returns the value stored in the `Result`. If the `Result` represents an error, an `Exception`
     * is thrown.
     *
     * @return A reference to the value stored in the `Result`.
     *
     * @throws mlib::err::Exception if the `Result` contains an error.
     */
    T& value()
    {
        if (is_error())
        {
            throw m_error;
        }

        return m_value;
    }

    /**
     * @brief Returns the value if it exists, or a default value if there is an error.
     *
     * This function returns the value stored in the `Result` if it is valid, otherwise it returns the specified
     * default value.
     *
     * @param default_value The default value to return if the `Result` is an error.
     * @return The value stored in the `Result`, or the default value if there is an error.
     */
    template<typename U = T>
    [[nodiscard]] T value_or(U&& default_value) const
    {
        if (is_value())
        {
            return m_value;
        }
        return std::forward<U>(default_value);
    }

    /**
     * @brief Retrieves the error code if the `Result` represents an error.
     *
     * This function returns the error code associated with the `Result`. If the `Result` is valid (i.e., contains
     * a value), it returns `EVERYTHING_FINE`.
     *
     * @return The error code associated with the `Result`, or `EVERYTHING_FINE` if there is no error.
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
     * @brief Destructor for the `Result` object.
     *
     * The destructor cleans up the resources used by the `Result`. If the `Result` contains a value, the value
     * is destroyed properly.
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
        T m_value; ///< The value stored in the `Result` (if valid).
        Exception m_error; ///< The error associated with the `Result` (if there is an error).
    };

    bool m_ok; ///< Flag indicating whether the `Result` contains a valid value or an error.
};

} // namespace err
} // namespace mlib

#endif // MLIB_RESULT_HPP
