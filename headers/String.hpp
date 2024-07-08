#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

#include <iostream>
#include <cstring>
#include "Utils.hpp"

namespace mlib {

template<size_t DefaultCapacity, size_t GrowFactor>
class StringBuffer final
{
private:
    char*  m_buf;
    size_t m_capacity;
public:
    char*        RawPtr()       noexcept { return m_buf; }
    const char*  RawPtr() const noexcept { return m_buf; }
    size_t       Length;
    Utils::Error Error = Utils::Error();
private:
    StringBuffer(size_t capacity, size_t length, const char* buf)
        : m_buf(new char[capacity]{}), m_capacity(capacity), Length(length)
    {
        if (!m_buf)
            Error = CREATE_ERROR(Utils::ErrorCode::ERROR_NO_MEMORY);
        if (buf)
            std::memcpy(m_buf, buf, length);
    }
public:
    explicit StringBuffer(const char* buf = nullptr)
        : StringBuffer(DefaultCapacity, 0, buf) {}

    explicit StringBuffer(size_t hintLength, const char* buf = nullptr)
        : StringBuffer(StringBuffer::calculateCapacity(hintLength), hintLength, buf) {}
public:
    StringBuffer(const StringBuffer& other)
        : StringBuffer(other.m_capacity, other.Length)
    {
        if (Error) return;

        std::memcpy(m_buf, other.m_buf, Length);
    }

    StringBuffer(StringBuffer&& other) noexcept
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

    StringBuffer& operator=(StringBuffer&& other) noexcept
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
    char& operator[](size_t index) & noexcept
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const & noexcept
    {
        return m_buf[index];
    }

    friend std::ostream& operator<<(std::ostream& out, const StringBuffer& buffer)
    {
        out << buffer.m_buf;
        return out;
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

template<size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class String final
{
private:
    StringBuffer<DefaultCapacity, GrowFactor> m_buf;
public:
    size_t       Length() { return m_buf.Length; }
    Utils::Error Error()  { return m_buf.Error;  }
public:
    String()
        : m_buf() {}

    String(size_t hintLength)
        : m_buf(hintLength) {}

    String(const char* string, size_t length)
        : m_buf(length, string) {}

    String(const char* string)
        : String(string, strlen(string)) {}

public:
    char& operator[](size_t index) &
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const &
    {
        return m_buf[index];
    }

    String& operator+=(const String& other)
    {
        std::size_t newLength = this->Length() + other.Length();

        m_buf.Realloc(newLength);
        if (Error()) return *this;

        std::memcpy(m_buf.RawPtr() + this->Length(), other.m_buf.RawPtr(), other.Length());

        return *this;
    }

    friend String operator+(const String& lhs, const String& rhs)
    {
        String<DefaultCapacity, GrowFactor> result = lhs;
        lhs += rhs;

        return result;
    }

    friend std::ostream& operator<<(std::ostream& out, const String& string)
    {
        return out << string.m_buf;
    }
};

} // namespace mlib

#endif
