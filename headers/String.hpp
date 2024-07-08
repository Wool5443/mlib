#ifndef MY_STLIB_STRING_HPP
#define MY_STLIB_STRING_HPP

#include <iostream>
#include <cstring>
#include "Utils.hpp"

namespace mlib
{
template<size_t DefCapacity, size_t GrowFactor>
class StringBuffer final
{
private:
    char*  m_buf;
    size_t m_capacity;
public:
    size_t       Length;
    Utils::Error Error = Utils::Error();
private:
    StringBuffer(size_t capacity, size_t length)
        : m_buf(new char[capacity]{}), m_capacity(capacity), Length(length)
    {
        if (!m_buf)
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
    }
public:
    StringBuffer()
        : StringBuffer(DefCapacity, 0) {}

    StringBuffer(size_t hintLength)
        : StringBuffer(StringBuffer::calculateCapacity(hintLength), hintLength) {}

    StringBuffer(const StringBuffer& other)
        : StringBuffer(other.m_capacity, other.Length)
    {
        if (Error) return;

        std::memcpy(m_buf, other.m_buf, Length);
    }

    StringBuffer(StringBuffer&& other)
        : m_buf(other.m_buf), m_capacity(other.m_capacity),
          Length(other.Length)
    {
        other.m_buf = nullptr;
    }

    StringBuffer& operator=(const StringBuffer& other)
    {
        if (this == &other) return *this;

        char* newBuf = new char[other.m_capacity]{};
        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
            return *this;
        }

        std::memcpy(newBuf, other.m_buf, other.Length);

        delete[] m_buf;

        m_buf      = newBuf;
        m_capacity = other.m_capacity;
        Length     = other.Length;
        Error      = Utils::Error();
    }

    StringBuffer& operator=(StringBuffer&& other)
    {
        std::swap(m_buf, other.m_buf);
        m_capacity = other.m_capacity;
        Length     = other.Length;
        Error      = other.Error;

        return *this;
    }

    ~StringBuffer()
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

        char* newBuf = new char[newCapacity]{};

        if (!newBuf)
        {
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
            return;
        }

        std::memcpy(newBuf, m_buf, Length);

        m_buf      = newBuf;
        m_capacity = newCapacity;

        delete[] m_buf;
    }
public:
    char& operator[](size_t index) &
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const &
    {
        return m_buf[index];
    }

    friend std::ostream& operator<<(std::ostream& out, const StringBuffer& buffer)
    {
        out << buffer.m_buf;
        return out;
    }
private:
    static inline size_t calculateCapacity(size_t hintLength)
    {
        size_t capacity = DefCapacity;

        while (capacity <= hintLength)
            capacity *= GrowFactor;

        return capacity;
    }
};
}

#endif
