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
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    Buffer<T, DefaultCapacity, GrowFactor> m_data{};
public:
    std::size_t  length = 0; ///< length
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    Utils::Error Error()  const noexcept  { return m_data.error;  }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR and =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new Vector object
     */
    Vector()
        : m_data() {}

    /**
     * @brief Construct a new Vector object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param [in] hintLength expected number of elements
     */
    explicit Vector(std::size_t hintLength)
        : m_data(hintLength) {}
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    static Utils::Result<Vector> New(std::size_t hintLength = DefaultCapacity)
    {
        Vector vec(hintLength);

        return { vec, vec.Error() };
    }

    static Utils::Result<Vector> New(const Vector& other)
    {
        Vector vec(other);

        return { vec, vec.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    using iterator      = T*;
    using constIterator = const T*;

          T& operator[](std::size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](std::size_t index) const & noexcept { return m_data[index]; }

    /**
     * @brief Returns the start of a buffer
     * 
     * @return iterator 
     */
    inline iterator      Begin()        noexcept { return m_data.RawPtr();          }

    /**
     * @brief Returns the start of a const buffer
     * 
     * @return constIterator 
     */
    inline constIterator CBegin() const noexcept { return m_data.RawPtr();          }

    /**
     * @brief Returns the end of a buffer
     * 
     * @return iterator 
     */
    inline iterator      End()          noexcept { return m_data.RawPtr() + length; }

    /**
     * @brief Returns the end of a const buffer
     * 
     * @return constIterator 
     */
    inline constIterator CEnd()   const noexcept { return m_data.RawPtr() + length; }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Pushes an element at the end of the vector
     * 
     * @param element 
     * 
     * @return Utils::Error 
     */
    Utils::Error PushBack(const T& element)
    {
        RETURN_ERROR(m_data.Realloc(length + 1));

        m_data[length] = element;

        length++;

        return {};
    }

    /**
     * @brief Pushes an element at the end of the vector
     * 
     * @param element 
     * 
     * @return Utils::Error 
     */
    Utils::Error PushBack(T&& element)
    {
        RETURN_ERROR(m_data.Realloc(length + 1));

        m_data[length] = element;

        length++;

        return Utils::Error();
    }
};

} //namespace mlib

#endif
