#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

#include <iostream>
#include <cstring>
#include "Utils.hpp"
#include "Vector.hpp"

namespace mlib {

template<std::size_t DefaultCapacity = 8, std::size_t GrowFactor = 2>
class String final
{
private:
    Buffer<char, DefaultCapacity, GrowFactor> m_buf;
public:
    std::size_t       Length() const noexcept { return m_buf.Length; }
    Utils::Error Error()  const noexcept { return m_buf.Error;  }
    char*        RawPtr()       noexcept { return m_buf.RawPtr(); }
    const char*  RawPtr() const noexcept { return m_buf.RawPtr(); }
public:
    String()
        : m_buf() {}

    explicit String(std::size_t hintLength)
        : m_buf(hintLength) {}

    String(const char* string, std::size_t length)
        : m_buf(length, string) {}

    String(const char* string)
        : String(string, strlen(string)) {}
    String(const char chr)
        : String()
    {
        *this += chr;
    }
public:
    operator char*()             noexcept { return RawPtr(); }
    operator const char*() const noexcept { return RawPtr(); }
    operator bool()        const noexcept { return Error();        }

    char& operator[](std::size_t index) & noexcept
    {
        return m_buf[index];
    }

    const char& operator[](std::size_t index) const & noexcept
    {
        return m_buf[index];
    }
private:
    String& append(const char* string, std::size_t length)
    {
        std::size_t oldLength = Length();
        std::size_t newLength = oldLength + length;

        m_buf.Realloc(newLength);
        if (Error()) return *this;

        std::memcpy(RawPtr() + oldLength, string, length);

        return *this;
    }
public:
    bool operator==(const String& other) const noexcept
    {
        return strncmp(RawPtr(), other.m_buf.RawPtr(),
                       std::min(Length(), other.Length())) == 0;
    }

    bool operator!=(const String& other) const noexcept
    {
        return !operator==(other);
    }

    String& operator+=(const char* other)
    {
        return append(other, strlen(other));
    }

    String& operator+=(const String& other)
    {
        return append(other.RawPtr(), other.Length());
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
        return out << string.RawPtr();
    }
public:
    Utils::Result<std::size_t> Find(char chr) const noexcept
    {
        const char* buf   = RawPtr();
        const char* found = strchr(buf, chr);
        if (!found)
            return {
                Utils::SIZET_POISON, CREATE_ERROR(Utils::ERROR_NOT_FOUND)
            };
        return found - buf;
    }

    Utils::Result<std::size_t> Find(const char* string) const noexcept
    {
        SoftAssertResult(string, Utils::SIZET_POISON, Utils::ERROR_NULLPTR);

        const char* buf   = RawPtr();
        const char* found = strstr(buf, string);

        if (!found)
            return { Utils::SIZET_POISON, CREATE_ERROR(Utils::ERROR_NOT_FOUND) };

        return { found - buf, Utils::Error() };
    }

    Utils::Result<std::size_t> Count(char chr) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), Utils::SIZET_POISON);

        std::size_t count = 0;

        for (std::size_t i = 0; i < Length(); i++)
            if (m_buf[i] == chr)
                count++;

        return { count, Utils::Error() };
    }

    Utils::Result<std::size_t> Count(const char* string) const noexcept
    {
        if (!string)
            return { Utils::SIZET_POISON,
                     CREATE_ERROR(Utils::ERROR_NULLPTR) };

        std::size_t      count = 0;
        const char* found = strstr(RawPtr(), string);

        while (found)
        {
            count++;
            found = strstr(found + 1, string);
        }

        return { count, Utils::Error() };
    }

    Utils::Result<std::size_t> Count(const String& string) const noexcept
    {
        return Count(string.RawPtr());
    }

    static constexpr const char* SPACE_CHARS = " \n\t\r\f\v";

    Utils::Result<Vector<String>> Split() const noexcept
    {
        return Split(SPACE_CHARS);
    }

    Utils::Result<Vector<String>> Split(const String& delimeters) const noexcept
    {
        return Split(delimeters.RawPtr());
    }

    Utils::Result<Vector<String>> Split(const char* delimiters) const noexcept
    {
        char* buf = strdup(RawPtr());

        if (!buf)
            return { {}, CREATE_ERROR(Utils::ERROR_NO_MEMORY) };

        Vector<String> words;
        RETURN_ERROR_RESULT(words.Error(), {});

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
