#ifndef MLIB_VECTOR_HPP
#define MLIB_VECTOR_HPP

/**
 * @file Vector.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains an implementation of a templated vector
 *
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 */

#include <cstdint>
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
    std::size_t    length = 0; ///< length
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Get Error
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Error()  const   noexcept  { return m_data.error;    }

    /**
     * @brief return internal buffer as char*
     *
     * @return char*
     */
    char*          RawPtr()       & noexcept  { return m_data.RawPtr(); }

    /**
     * @brief return internal buffer as const char*
     *
     * @return const char*
     */
    const char*    RawPtr() const & noexcept  { return m_data.RawPtr(); }
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
    static err::Result<Vector> New(std::size_t hintLength = DefaultCapacity)
    {
        Vector vec(hintLength);
        LOG_ERROR_IF(vec.Error());
        return { vec, vec.Error() };
    }

    static err::Result<Vector> New(const Vector& other)
    {
        Vector vec(other);
        LOG_ERROR_IF(vec.Error());
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
    iterator      begin()        & noexcept { return m_data.RawPtr();          }

    /**
     * @brief Returns the start of a const buffer
     *
     * @return constIterator
     */
    constIterator begin()  const & noexcept { return m_data.RawPtr();          }

    /**
     * @brief Returns the start of a const buffer
     *
     * @return constIterator
     */
    constIterator cbegin() const & noexcept { return m_data.RawPtr();          }

    /**
     * @brief Returns the end of a buffer
     *
     * @return iterator
     */
     iterator     end()         & noexcept { return m_data.RawPtr() + length;  }

    /**
     * @brief Returns the end of a const buffer
     *
     * @return constIterator
     */
    constIterator end()    const & noexcept { return m_data.RawPtr() + length; }

    /**
     * @brief Returns the end of a const buffer
     *
     * @return constIterator
     */
    constIterator cend()   const & noexcept { return m_data.RawPtr() + length; }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Pushes an element at the end of the vector
     *
     * @param [in] element
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushBack(const T& element)
    {
        if (length == m_data.GetCapacity())
            RETURN_ERROR(m_data.Realloc(length + 1));

        m_data[length++] = element;

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Pushes an element at the end of the vector
     *
     * @param [in] element
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushBack(T&& element)
    {
        if (length == m_data.GetCapacity())
            RETURN_ERROR(m_data.Realloc(length + 1));

        m_data[length++] = std::move(element);

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Creates an element at the end of the vector
     *
     * @tparam Args
     *
     * @param [in] args ctor args for a new element of T
     *
     * @return err::ErrorCode
     */
    template<typename... Args>
    err::ErrorCode EmplaceBack(Args&&... args)
    {
        if (length == m_data.GetCapacity())
            RETURN_ERROR(m_data.Realloc(length + 1));

        new(&m_data[length++]) T(std::forward<Args>(args)...);

        return err::EVERYTHING_FINE;
    }

    err::Result<std::size_t> Find(const T& value)
    {
        for (std::size_t i = 0; i < length; i++)
        {
            if (m_data[i] == value)
                return { i, {} };
        }

        return { SIZE_MAX, err::ERROR_NOT_FOUND };
    }
};

template<typename T>
struct Hash<Vector<T>>
{
    uint64_t operator()(const Vector<T>& vector)
    {
        return CRC32(vector.RawPtr(), vector.length);
    }
};

} //namespace mlib

#endif
