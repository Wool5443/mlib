#ifndef MLIB_VECTOR_HPP
#define MLIB_VECTOR_HPP

/**
 * @file Vector.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains an implementation of a templated vector
 * 
 * @version 1.0
 * @date 16-07-2024
 * 
 * @copyright Copyright (c) 2024
 */

#include "GeneralBuffer.hpp"

namespace mlib {

/** @class Vector
 * @brief Templated vector
 * 
 * @tparam T value type
 * @tparam DefaultCapacity 
 * @tparam GrowFactor 
 */
template<typename T, std::size_t DefaultCapacity = 8, std::size_t GrowFactor = 2>
class Vector final
{
private:
    Buffer<T, DefaultCapacity, GrowFactor> m_buf;
public:
    std::size_t  Length() const noexcept  { return m_buf.Length; }
    Utils::Error Error()  const noexcept  { return m_buf.Error;  }
public:
    /**
     * @brief Construct a new Vector object
     */
    Vector()
        : m_buf() {}
    /**
     * @brief Construct a new Vector object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param [in] hintLength expected number of elements
     */
    explicit Vector(std::size_t hintLength)
        : m_buf(hintLength) {}
public:
    /**
     * @brief Pushes an element at the end of the vector
     * 
     * @param element 
     * @return Utils::Error 
     */
    Utils::Error PushBack(const T& element)
    {
        std::size_t oldLength = Length();
        RETURN_ERROR(m_buf.Realloc(oldLength + 1));

        m_buf[oldLength] = element;

        return Utils::Error();
    }

    /**
     * @brief Pushes an element at the end of the vector
     * 
     * @param element 
     * @return Utils::Error 
     */
    Utils::Error PushBack(T&& element)
    {
        std::size_t oldLength = Length();
        RETURN_ERROR(m_buf.Realloc(oldLength + 1));

        m_buf[oldLength] = element;

        return Utils::Error();
    }
public:
    T& operator[](std::size_t index) & noexcept
    {
        return m_buf[index];
    }

    const T& operator[](std::size_t index) const & noexcept
    {
        return m_buf[index];
    }
};

} //namespace mlib

#endif
