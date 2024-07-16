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
private:
    T*          m_buf;
    std::size_t m_capacity;
public:
    std::size_t  Length; ///< Number of elements in the buffer
    Utils::Error Error = Utils::Error(); ///< Buffer error
public:
    /**
     * @brief returns the raw buffer
     * 
     * @return T* raw buffer
     */
    inline T*            RawPtr()            noexcept { return m_buf;      }
    /**
     * @brief returns the raw buffer
     * 
     * @return T* raw buffer
     */
    inline const T*      RawPtr()      const noexcept { return m_buf;      }
    /**
     * @brief Get the Capacity object
     * 
     * @return std::size_t 
     */
    inline std::size_t   GetCapacity() const noexcept { return m_capacity; }
private:
    Buffer(std::size_t capacity, std::size_t length, const char* buf)
        : m_buf(new T[capacity]{}), m_capacity(capacity), Length(length)
    {
        HardAssert(length < capacity, Utils::ERROR_BAD_NUMBER);

        if (!m_buf)
        {
            Error = CREATE_ERROR(Utils::ERROR_NO_MEMORY);
            return;
        }
        if (buf)
            std::memcpy((char*)m_buf, buf, length);
    }
public:
    /**
     * @brief Construct a new Buffer object
     */
    explicit Buffer()
        : Buffer(DefaultCapacity, 0, nullptr) {}
    /**
     * @brief Construct a new Buffer object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param [in] hintLength length to ensure big enough capacity
     * @param [in] buf can be used for buffers
     * of type without a ctor, because if passed
     * it will std::memcpy buf. Also sets the length to hintLength
     */
    explicit Buffer(std::size_t hintLength, const char* buf = nullptr)
        : Buffer(Buffer::calculateCapacity(hintLength), buf ? hintLength : 0, buf) {}
public:
    /**
     * @brief Buffer copy constructor
     * 
     * @param [in] other buffer to copy
     */
    Buffer(const Buffer& other)
        : Buffer(other.m_capacity, other.Length, nullptr)
    {
        for (std::size_t i = 0; i < other.Length; i++)
            m_buf[i] = other[i];
    }

    /**
     * @brief Buffer move constructor
     * 
     * @param [in] other buffer to move
     */
    Buffer(Buffer&& other) noexcept
        : m_buf(other.m_buf), m_capacity(other.m_capacity),
          Length(other.Length)
    {
        other.m_buf = nullptr;
    }

    /**
     * @brief Buffer copy assignment
     * 
     * @param [in] other buffer to copy
     * @return Buffer&
     */
    Buffer& operator=(const Buffer& other)
    {
        if (this == &other) return *this;

        T* newBuf = new T[other.m_capacity]{};

        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ERROR_NO_MEMORY);
            return *this;
        }

        for (std::size_t i = 0; i < other.Length; i++)
            newBuf[i] = other[i];

        delete[] m_buf;

        m_buf      = newBuf;
        m_capacity = other.m_capacity;
        Length     = other.Length;
        Error      = Utils::Error();

        return *this;
    }

    /**
     * @brief Buffer move assignment
     * 
     * @param [in] other buffer to move
     * @return Buffer&
     */
    Buffer& operator=(Buffer&& other) noexcept
    {
        std::swap(m_buf, other.m_buf);
        m_capacity = other.m_capacity;
        Length     = other.Length;
        Error      = other.Error;

        return *this;
    }

    virtual ~Buffer()
    {
        delete[] m_buf;
    }
public:
    /**
     * @brief Reallocates the buffer and changes its length
     * 
     * @param newLength
     * @return Utils::Error 
     */
    Utils::Error Realloc(std::size_t newLength)
    {
        if (Error)
            return Error;

        std::size_t oldLength = Length;
        Length                = newLength;

        if (newLength < m_capacity)
            return Utils::Error();

        std::size_t newCapacity = calculateCapacity(newLength);

        T* newBuf = new T[newCapacity]{};

        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ERROR_NO_MEMORY);
            return Error;
        }

        for (std::size_t i = 0; i <= oldLength; i++)
            newBuf[i] = m_buf[i];

        delete[] m_buf;

        m_buf      = newBuf;
        m_capacity = newCapacity;

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
private:
    static inline std::size_t calculateCapacity(std::size_t hintLength) noexcept
    {
        std::size_t capacity = DefaultCapacity;

        while (capacity <= hintLength)
            capacity *= GrowFactor;

        return capacity;
    }
};

} // namespace mlib

#endif
