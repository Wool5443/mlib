#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

/**
 * @file String.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains the implementation of a String
 * @version 1.0
 * @date 16-07-2024
 * 
 * @copyright Copyright (c) 2024
 */

#include <iostream>
#include <cstring>
#include "Utils.hpp"
#include "Vector.hpp"

namespace mlib {

/** @class String
 * @brief A resizeable string
 * 
 * @tparam DefaultCapacity
 * @tparam GrowFactor
 */
template<std::size_t DefaultCapacity = 8, std::size_t GrowFactor = 2>
class String final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    Buffer<char, DefaultCapacity, GrowFactor> m_data{true};
public:
    std::size_t length = 0; ///< string length
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Get error
     * 
     * @return Utils::Error error
     */
    Utils::Error Error()  const noexcept { return m_data.error;    }
    /**
     * @brief Get a c-style string
     * 
     * @return char* string
     */
    char*        RawPtr()       noexcept { return m_data.RawPtr(); }
    /**
     * @brief Get a c-style string
     * 
     * @return const char* string
     */
    const char*  RawPtr() const noexcept { return m_data.RawPtr(); }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR and =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new String object
     */
    String()
        : m_data() {}

    /**
     * @brief Construct a new String object
     * with known length
     * 
     * @param [in] hintLength legnth to ensure capacity
     * for less reallocations
     */
    explicit String(std::size_t hintLength)
        : m_data(hintLength) {}

    /**
     * @brief Construct a new String object
     * from a c-style string knowing its length
     * 
     * @param [in] string 
     * @param [in] length 
     */
    String(const char* string, std::size_t length)
        : m_data(length), length(length)
    {
        if (Error()) return;
        std::memcpy(RawPtr(), string, length);
    }

    /**
     * @brief Construct a new String object
     * from a c-style string
     * 
     * @param [in] string 
     */
    String(const char* string)
        : String(string, strlen(string)) {}

    /**
     * @brief Construct a new String object
     * from a character
     * 
     * @param [in] chr 
     */
    String(const char chr)
        : String(&chr, 1) {}
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Constructs a new String object
     * from a cstring
     * 
     * @param string 
     * 
     * @return Utils::Result<String> 
     */
    static Utils::Result<String> New(const char* string)
    {
        String str(string);

        return { str, str.Error() };
    }

    /**
     * @brief Constructs a new String object
     * from a cstring knowing its length
     * 
     * @param string 
     * 
     * @return Utils::Result<String> 
     */
    static Utils::Result<String> New(const char* string, std::size_t length)
    {
        String str(string, length);

        return { str, str.Error() };
    }

    /**
     * @brief Construct a new String object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     * 
     * @param hintLength length to ensure big enough capacity
     * for less reallocations
     * 
     * @return Utils::Result<String> 
     */
    static Utils::Result<String> New(std::size_t hintLength = DefaultCapacity)
    {
        String str(hintLength);

        return { str, str.Error() };
    }

    /**
     * @brief Construct a new String object
     * by copying
     * 
     * @param other string to copy
     * 
     * @return Utils::Result<String> 
     */
    static Utils::Result<String> New(const String& other)
    {
        String str(other);

        return { str, str.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    using iterator      = char*;
    using constIterator = const char*;

    char& operator[](std::size_t index) & noexcept
    {
        return m_data[index];
    }

    const char& operator[](std::size_t index) const & noexcept
    {
        return m_data[index];
    }

    /**
     * @brief Returns the start of a string
     * 
     * @return iterator 
     */
    inline iterator      Begin()        noexcept { return RawPtr();          }

    /**
     * @brief Returns the start of a const string
     * 
     * @return constIterator 
     */
    inline constIterator CBegin() const noexcept { return RawPtr();          }

    /**
     * @brief Returns the end of a string
     * 
     * @return iterator 
     */
    inline iterator      End()          noexcept { return RawPtr() + length; }

    /**
     * @brief Returns the end of a const string
     * 
     * @return constIterator 
     */
    inline constIterator CEnd()   const noexcept { return RawPtr() + length; }
///////////////////////////////////////////////////////////////////////////////
//
//                              MATH OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    bool operator==(const String& other) const noexcept
    {
        return strncmp(RawPtr(), other.m_data.RawPtr(),
                       std::min(length, other.length)) == 0;
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
        return append(other.RawPtr(), other.length);
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
private:
    String& append(const char* string, std::size_t strLength)
    {
        std::size_t oldLength = length;
        std::size_t newLength = oldLength + strLength;

        m_data.Realloc(newLength);
        if (Error()) return *this;

        std::memcpy(RawPtr() + oldLength, string, length);

        return *this;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              STREAM OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    friend std::ostream& operator<<(std::ostream& out, const String& string)
    {
        return out << string.RawPtr();
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              IMPLICIT CASTS
//
///////////////////////////////////////////////////////////////////////////////
public:
    operator char*()              noexcept { return RawPtr(); }
    operator const char*()  const noexcept { return RawPtr(); }
    operator bool()         const noexcept { return Error();  }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Finds a char and returns its index
     * 
     * @param [in] chr char to find
     * 
     * @return Utils::Result<std::size_t> index result
     */
    Utils::Result<std::size_t> Find(char chr) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), Utils::SIZET_POISON);

        const char* buf   = RawPtr();
        const char* found = strchr(buf, chr);
        if (!found)
            return {
                Utils::SIZET_POISON, CREATE_ERROR(Utils::ERROR_NOT_FOUND)
            };

        return found - buf;
    }

    /**
     * @brief Finds a substring and returns its index
     * 
     * @param [in] string 
     * 
     * @return Utils::Result<std::size_t> index result
     */
    Utils::Result<std::size_t> Find(const char* string) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), Utils::SIZET_POISON);

        SoftAssertResult(string, Utils::SIZET_POISON, Utils::ERROR_NULLPTR);

        const char* data  = RawPtr();
        const char* found = strstr(data, string);

        if (!found)
            return { Utils::SIZET_POISON, CREATE_ERROR(Utils::ERROR_NOT_FOUND) };

        return { found - data, {} };
    }

    /**
     * @brief Counts occurences of chr
     * 
     * @param [in] chr char to count
     * 
     * @return Utils::Result<std::size_t> count result
     */
    Utils::Result<std::size_t> Count(char chr) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), Utils::SIZET_POISON);

        std::size_t count = 0;

        for (std::size_t i = 0; i < length; i++)
            if (m_data[i] == chr)
                count++;

        return { count, Utils::Error() };
    }

    /**
     * @brief Counts occurences of string
     * 
     * @param [in] string string to count
     * 
     * @return Utils::Result<std::size_t> count result
     */
    Utils::Result<std::size_t> Count(const char* string) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), Utils::SIZET_POISON);

        if (!string)
            return { Utils::SIZET_POISON,
                     CREATE_ERROR(Utils::ERROR_NULLPTR) };

        std::size_t count = 0;
        const char* found = strstr(RawPtr(), string);

        while (found)
        {
            count++;
            found = strstr(found + 1, string);
        }

        return { count, Utils::Error() };
    }

    /**
     * @brief Counts occurences of string
     * 
     * @param [in] string string to count
     * 
     * @return Utils::Result<std::size_t> count result
     */
    Utils::Result<std::size_t> Count(const String& string) const noexcept
    {
        return Count(string.RawPtr());
    }

private:
    static constexpr const char* SPACE_CHARS = " \n\t\r\f\v";
public:
    /**
     * @brief Split the string by space characters
     * 
     * @return Utils::Result<Vector<String>> vector of strings
     */
    Utils::Result<Vector<String>> Split() const noexcept
    {
        return Split(SPACE_CHARS);
    }

    /**
     * @brief Split the string by delimeters
     * 
     * @param [in] delimeters what to split by
     * 
     * @return Utils::Result<Vector<String>> vector of strings
     */
    Utils::Result<Vector<String>> Split(const String& delimeters) const noexcept
    {
        return Split(delimeters.RawPtr());
    }

    /**
     * @brief Split the string by delimeters
     * 
     * @param [in] delimeters what to split by
     * 
     * @return Utils::Result<Vector<String>> vector of strings
     */
    Utils::Result<Vector<String>> Split(const char* delimiters) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), {});

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

    /**
     * @brief Filters out characters
     * 
     * @param [in] filter characters to filter
     * 
     * @return Utils::Error 
     */
    Utils::Error Filter(const char* filter) noexcept
    {
        RETURN_ERROR(Error());

        SoftAssert(filter, Utils::ErrorCode::ERROR_NULLPTR);

        char*       writePtr = RawPtr();
        const char* readPtr  = writePtr;

        while (*readPtr)
        {
            char c = *readPtr++;

            if (!std::strchr(filter, c))
                *writePtr++ = c;
        }

        *writePtr = '\0';

        return Utils::Error();
    }

    /**
     * @brief Filters out characters
     * 
     * @param [in] filter characters to filter
     * 
     * @return Utils::Error 
     */
    Utils::Error Filter(const String& filter) noexcept
    {
        return Filter(filter.RawPtr());
    }

    /**
     * @brief Filters out space characters
     * 
     * @return Utils::Error 
     */
    Utils::Error Filter() noexcept
    {
        return Filter(SPACE_CHARS);
    }
};

} // namespace mlib

#endif
