#ifndef MLIB_GENERAL_BUFFER
#define MLIB_GENERAL_BUFFER

#include <iostream>
#include <cstring>
#include "Utils.hpp"

namespace mlib {

template<typename T, size_t DefaultCapacity, size_t GrowFactor>
class Buffer final
{
protected:
    T*     m_buf;
    size_t m_capacity;
public:
    char*        RawPtr()       noexcept { return m_buf; }
    const char*  RawPtr() const noexcept { return m_buf; }
    size_t       Length;
    Utils::Error Error = Utils::Error();
private:
    Buffer(size_t capacity, size_t length, const char* buf)
        : m_buf(new T[capacity]{}), m_capacity(capacity), Length(length)
    {
        HardAssert(length < capacity, Utils::ErrorCode::ERROR_BAD_NUMBER);

        if (!m_buf)
        {
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
            return;
        }
        if (buf)
            std::memcpy(m_buf, buf, length);
    }
public:
    explicit Buffer(const char* buf = nullptr)
        : Buffer(DefaultCapacity, 0, buf) {}

    explicit Buffer(size_t hintLength, const char* buf = nullptr)
        : Buffer(Buffer::calculateCapacity(hintLength), hintLength, buf) {}
public:
    Buffer(const Buffer& other)
        : Buffer(other.m_capacity, other.Length, nullptr)
    {
        for (size_t i = 0; i < other.Length; i++)
            m_buf[i] = other[i];
    }

    Buffer(Buffer&& other) noexcept
        : m_buf(other.m_buf), m_capacity(other.m_capacity),
          Length(other.Length)
    {
        other.m_buf = nullptr;
    }

    Buffer& operator=(const Buffer& other)
    {
        if (this == &other) return *this;

        T* newBuf = new T[other.m_capacity]{};

        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
            return *this;
        }

        for (size_t i = 0; i < other.Length; i++)
            newBuf[i] = other[i];

        delete[] m_buf;

        m_buf      = newBuf;
        m_capacity = other.m_capacity;
        Length     = other.Length;
        Error      = Utils::Error();
    }

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
    void Realloc(size_t newLength)
    {
        if (Error)
        {
            Error.Print();
            return;
        }

        if (newLength < m_capacity)
            return;

        size_t newCapacity = calculateCapacity(newLength);

        T* newBuf = new T[newCapacity]{};

        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
            return;
        }

        for (size_t i = 0; i < Length; i++)
            newBuf[i] = m_buf[i];

        m_buf      = newBuf;
        m_capacity = newCapacity;

        delete[] m_buf;
    }
public:
    char& operator[](size_t index) & noexcept
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const & noexcept
    {
        return m_buf[index];
    }
private:
    static inline size_t calculateCapacity(size_t hintLength) noexcept
    {
        size_t capacity = DefaultCapacity;

        while (capacity <= hintLength)
            capacity *= GrowFactor;

        return capacity;
    }
};

} // namespace mlib

#endif
