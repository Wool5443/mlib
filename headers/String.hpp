#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

#include <iostream>
#include <cstring>
#include "Utils.hpp"
#include "GeneralBuffer.hpp"

namespace mlib {

template<size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class String final
{
private:
    // StringBuffer<DefaultCapacity, GrowFactor> m_buf;
    Buffer<char, DefaultCapacity, GrowFactor> m_buf;
public:
    size_t       Length() const noexcept { return m_buf.Length; }
    Utils::Error Error()  const noexcept { return m_buf.Error;  }
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
    char& operator[](size_t index) & noexcept
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const & noexcept
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
        return out << string.m_buf.RawPtr();
    }
};

} // namespace mlib

#endif
