#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

#include <iostream>
#include <cstring>
#include "Utils.hpp"
#include "Vector.hpp"

namespace mlib {

template<size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class String final
{
private:
    Buffer<char, DefaultCapacity, GrowFactor> m_buf;
public:
    size_t       Length() const noexcept { return m_buf.Length; }
    Utils::Error Error()  const noexcept { return m_buf.Error;  }
    char*        RawPtr()       noexcept { return static_cast<char*>(*this);       }
    const char*  RawPtr() const noexcept { return static_cast<const char*>(*this); }
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
    operator char*()             noexcept { return m_buf.RawPtr(); }
    operator const char*() const noexcept { return m_buf.RawPtr(); }
    operator bool()        const noexcept { return Error();        }

    char& operator[](size_t index) & noexcept
    {
        return m_buf[index];
    }

    const char& operator[](size_t index) const & noexcept
    {
        return m_buf[index];
    }
private:
    String& append(const char* string, size_t length)
    {
        size_t newLength = this->Length() + length;

        m_buf.Realloc(newLength);
        if (Error()) return *this;

        std::memcpy(m_buf.RawPtr() + this->Length(), string, length);

        return *this;
    }
public:
    String& operator+=(const char* other)
    {
        return append(other, strlen(other));
    }

    String& operator+=(const String& other)
    {
        return append(other.m_buf.RawPtr(), other.Length());
    }

    #define OPERATOR_PLUS_CODE                              \
    {                                                       \
        String<> result{lhs};                               \
        result += rhs;                                      \
        return result;                                      \
    }
    friend String operator+(const char* lhs, const String& rhs)
    OPERATOR_PLUS_CODE
    friend String operator+(const String& lhs, const char* rhs)
    OPERATOR_PLUS_CODE
    friend String operator+(const String& lhs, const String& rhs)
    OPERATOR_PLUS_CODE
    #undef OPERATOR_PLUS_CODE

    friend std::ostream& operator<<(std::ostream& out, const String& string)
    {
        return out << string.m_buf.RawPtr();
    }
public:
    static constexpr const char* SPACE_CHARS = " \n\t\r\f\v";

    Vector<String> Split()
    {
        return Split(SPACE_CHARS);
    }

    Vector<String> Split(const String& delimeters)
    {
        return Split(delimeters.m_buf.RawPtr());
    }

    Vector<String> Split(const char* delimiters)
    {
        char* buf = strdup(m_buf.RawPtr());

        Vector<String> words;

        const char* token = strtok(buf, delimiters);

        std::size_t i = 0;

        while (token)
        {
            words.PushBack(token);
            token = strtok(nullptr, delimiters);
        }

        free(buf);

        return words;
    }
};

} // namespace mlib

#endif
