#ifndef MLIB_GENERAL_BUFFER
#define MLIB_GENERAL_BUFFER

/**
 * @file GeneralBuffer.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains the implementation of a templated
 * buffer used in other containers
 *
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Error.hpp"

namespace mlib {

/**
 * @brief Resizeable buffer allocated on heap
 *
 * @tparam T value type
 * @tparam DefaultCapacity
 * @tparam GrowFactor
 */
template<typename T>
class Buffer final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    T*           m_data     = nullptr;
    std::size_t  m_capacity = 0;
public:
    err::ErrorCode error    = err::ERROR_UNINITIALIZED; ///< Buffer error
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
     T*            RawPtr()            noexcept { return m_data;     }

    /**
     * @brief returns the raw buffer
     *
     * @return T* raw buffer
     */
     const T*      RawPtr()      const noexcept { return m_data;     }

    /**
     * @brief Get the Capacity object
     *
     * @return std::size_t
     */
     std::size_t   GetCapacity() const noexcept { return m_capacity; }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR and =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new empty Buffer object
     */
    Buffer() noexcept = default;

    /**
     * @brief Construct a Buffer
     *
     * @param [in] capacity
     */
    explicit Buffer(std::size_t capacity) noexcept
        : m_data(new T[capacity]{}), m_capacity(capacity), error(err::EVERYTHING_FINE)
    {
        if (!m_data)
        {
            error = err::ERROR_NO_MEMORY;
            LOG_ERROR(error);
        }
    }

    /**
     * @brief Buffer copy constructor
     *
     * @param [in] other buffer to copy
     */
    Buffer(const Buffer& other) noexcept
        : Buffer(other.m_capacity)
    {
        if (error)
        {
            LOG_ERROR(error);
            return;
        }

        std::copy(other.cebgin(), other.cend(), m_data);
    }

    /**
     * @brief Buffer move constructor
     *
     * @param [in] other buffer to move
     */
    Buffer(Buffer&& other) noexcept
        : m_data(other.m_data), m_capacity(other.m_capacity),
          error(other.error)
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
    Buffer& operator=(const Buffer& other) noexcept
    {
        if (this == &other) return *this;

        T* newData = new T[other.m_capacity]{};

        if (!newData)
        {
            error = err::ERROR_NO_MEMORY;
            LOG_ERROR(error);
            return *this;
        }

        std::copy(other.cebgin(), other.cend(), newData);

        delete[] m_data;

        m_data     = newData;
        m_capacity = other.m_capacity;
        error      = err::EVERYTHING_FINE;

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
        if (this == &other) return *this;

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
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a Buffer
     *
     * @param [in] capacity
     *
     * @return err::Result<Buffer>
     */
    static err::Result<Buffer> New(std::size_t capacity)
    noexcept
    {
        Buffer buffer(capacity);
        LOG_ERROR_IF(buffer.error);
        return { buffer, buffer.error };
    }

    /**
     * @brief Copies a buffer object
     *
     * @param [in] other buffer to copy
     *
     * @return err::Result<Buffer>
     */
    static err::Result<Buffer> New(const Buffer& other) noexcept
    {
        Buffer buffer(other);
        LOG_ERROR_IF(buffer.error);
        return { buffer, buffer.error };
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
     * @param [in] newCapacity
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Realloc(std::size_t newCapacity) noexcept
    {
        if (error == err::ERROR_UNINITIALIZED)
            error = err::EVERYTHING_FINE;
        else
            RETURN_ERROR(error);

        if (m_capacity >= newCapacity)
            return err::EVERYTHING_FINE;

        std::size_t capacity = calculateCapacity(m_capacity, newCapacity);

        T* newData = new T[capacity]{};

        if (!newData)
            RETURN_ERROR(err::ERROR_NO_MEMORY, error = err::ERROR_NO_MEMORY);

        std::move(cebgin(), cend(), newData);

        delete[] m_data;

        m_data     = newData;
        m_capacity = capacity;

        return err::EVERYTHING_FINE;
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
     iterator      begin()        & noexcept { return m_data; }

    /**
     * @brief Returns the start of a const buffer
     *
     * @return constIterator
     */
     constIterator cebgin() const & noexcept { return m_data; }

    /**
     * @brief Returns the end of a buffer
     *
     * @return iterator
     */
     iterator      end()          & noexcept { return m_data + m_capacity; }

    /**
     * @brief Returns the end of a const buffer
     *
     * @return constIterator
     */
     constIterator cend()   const & noexcept { return m_data + m_capacity; }
private:
    static  std::size_t calculateCapacity(std::size_t currentCapacity,
                                          std::size_t hintLength)
    noexcept
    {
        static const std::size_t growFactorNumerator   = 3;
        static const std::size_t growFactorDenominator = 2;
        static const std::size_t minCapacity           = 2;

        std::size_t capacity = std::max(currentCapacity, minCapacity);

        while (capacity < hintLength)
            capacity = capacity * growFactorNumerator /
                                  growFactorDenominator;

        return capacity;
    }
};

} // namespace mlib

#endif
