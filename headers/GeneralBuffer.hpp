#ifndef MLIB_GENERAL_BUFFER
#define MLIB_GENERAL_BUFFER

/**
 * @file GeneralBuffer.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains the implementation of a templated
 * buffer used in other containers
 * 
 * @version 1.0
 * @date 16-07-2024
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <cstring>
#include "Utils.hpp"

namespace mlib {

/**
 * @brief Resizeable buffer allocated on heap
 * 
 * @tparam T value type
 * @tparam DefaultCapacity
 * @tparam GrowFactor
 */
template<typename T, std::size_t DefaultCapacity, std::size_t GrowFactor>
class Buffer final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    T*          m_data     = nullptr;
    std::size_t m_capacity = 0;
public:
    Utils::Error error{}; ///< Buffer error
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief returns the raw buffer
     * 
     * @return T* raw buffer
     */
    inline T*            RawPtr()            noexcept { return m_data;     }

    /**
     * @brief returns the raw buffer
     * 
     * @return T* raw buffer
     */
    inline const T*      RawPtr()      const noexcept { return m_data;     }

    /**
     * @brief Get the Capacity object
     * 
     * @return std::size_t 
     */
    inline std::size_t   GetCapacity() const noexcept { return m_capacity; }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR and =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new Buffer object
     * with default capacity
     */
    Buffer()
        : Buffer(DefaultCapacity, true) {}

    /**
     * @brief Construct a new Buffer object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param [in] hintLength length to ensure big enough capacity
     * for less reallocations
     */
    explicit Buffer(std::size_t hintLength)
        : Buffer(calculateCapacity(hintLength), true) {}

    /**
     * @brief Buffer copy constructor
     * 
     * @param [in] other buffer to copy
     */
    Buffer(const Buffer& other)
        : Buffer(other.m_capacity, true)
    {
        if (error) return;

        std::copy(other.CBegin(), other.CEnd(), m_data);
    }

    /**
     * @brief Buffer move constructor
     * 
     * @param [in] other buffer to move
     */
    Buffer(Buffer&& other) noexcept
        : m_data(other.m_data), m_capacity(other.m_capacity)
    {
        other.m_data = nullptr;
    }

    /**
     * @brief Buffer copy assignment
     * 
     * @param [in] other buffer to copy
     * 
     * @return Buffer&
     */
    Buffer& operator=(const Buffer& other)
    {
        if (this == &other) return *this;

        T* newData = new T[other.m_capacity]{};

        if (!newData)
            return *this;

        std::copy(other.CBegin(), other.CEnd(), newData);

        delete[] m_data;

        m_data     = newData;
        m_capacity = other.m_capacity;
        error      = {};

        return *this;
    }

    /**
     * @brief Buffer move assignment
     * 
     * @param [in] other buffer to move
     * 
     * @return Buffer&
     */
    Buffer& operator=(Buffer&& other) noexcept
    {
        std::swap(m_data, other.m_data);
        m_capacity = other.m_capacity;
        error      = other.error;

        return *this;
    }

    /**
     * @brief Destroy the Buffer object
     */
    ~Buffer()
    {
        delete[] m_data;
    }
private:
    explicit Buffer(std::size_t capacity, bool isValidCapacity)
        : m_data(new T[capacity]{}), m_capacity(capacity)
    {
        HardAssert(isValidCapacity, Utils::ERROR_BAD_VALUE);

        if (!m_data)
            error = CREATE_ERROR(Utils::ERROR_NO_MEMORY);
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new Buffer object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param hintLength length to ensure big enough capacity
     * for less reallocations
     * 
     * @return Utils::Result<Buffer> 
     */
    static Utils::Result<Buffer> New(std::size_t hintLength = DefaultCapacity)
    {
        Buffer buffer(hintLength);

        return { buffer, buffer.error };
    }

    /**
     * @brief Copies a buffer object
     * 
     * @param other buffer to copy
     * @return Utils::Result<Buffer> 
     */
    static Utils::Result<Buffer> New(const Buffer& other)
    {
        Buffer buffer(other);

        return { buffer, buffer.error };
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
    inline iterator      Begin()        noexcept { return m_data; }

    /**
     * @brief Returns the start of a const buffer
     * 
     * @return constIterator 
     */
    inline constIterator CBegin() const noexcept { return m_data; }

    /**
     * @brief Returns the end of a buffer
     * 
     * @return iterator 
     */
    inline iterator      End()          noexcept { return m_data + m_capacity; }

    /**
     * @brief Returns the end of a const buffer
     * 
     * @return constIterator 
     */
    inline constIterator CEnd()   const noexcept { return m_data + m_capacity; }
private:
    static inline std::size_t calculateCapacity(std::size_t hintLength) noexcept
    {
        std::size_t capacity = DefaultCapacity;

        while (capacity < hintLength)
            capacity *= GrowFactor;

        return capacity;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Reallocates the buffer
     * new capacity is at least capacity
     * 
     * @param_capacity
     * @return Utils::Error 
     */
    Utils::Error Realloc(std::size_t capacity)
    {
        RETURN_ERROR(error);

        std::size_t newCapacity = calculateCapacity(capacity);

        T* newData = new T[newCapacity]{};

        if (!newData)
            return CREATE_ERROR(Utils::ERROR_NO_MEMORY);

        std::copy(CBegin(), CEnd(), newData);

        delete[] m_data;

        m_data     = newData;
        m_capacity = newCapacity;

        return {};
    }
};

} // namespace mlib

#endif
