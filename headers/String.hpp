#ifndef MLIB_STRING_HPP
#define MLIB_STRING_HPP

/**
 * @file String.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains the implementation of a String
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 */

#include <ostream>
#include <cstring>
#include <cstdint>
#include "Hash.hpp"
#include "Error.hpp"
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
    Buffer<char, DefaultCapacity, GrowFactor> m_data;
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
     * @return err::ErrorCode error
     */
    err::ErrorCode Error()  const noexcept { return m_data.error;  }

    /**
     * @brief Get a c-style string
     *
     * @return char* string
     */
    char*          RawPtr()       noexcept { return m_data.RawPtr(); }
    /**
     * @brief Get a c-style string
     *
     * @return const char* string
     */
    const char*    RawPtr() const noexcept { return m_data.RawPtr(); }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR and =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new String object
     */
    String() noexcept
        : m_data() {}

    /**
     * @brief Construct a new String object
     * with known length
     *
     * @param [in] hintLength legnth to ensure capacity
     * for less reallocations
     */
    explicit String(std::size_t hintLength) noexcept
        : m_data(hintLength + 1) {}

    /**
     * @brief Construct a new String object
     * from a c-style string knowing its length
     *
     * @param [in] string
     * @param [in] length
     */
    String(const char* string, std::size_t length) noexcept
        : m_data(length + 1), length(length)
    {
        if (auto err = Error())
        {
            LOG_ERROR(err);
            return;
        }

        std::memcpy(RawPtr(), string, length);
    }

    /**
     * @brief Construct a new String object
     * from a c-style string
     *
     * @param [in] string
     */
    String(const char* string) noexcept
        : String(string, strlen(string)) {}

    /**
     * @brief Construct a new String object
     * from a character
     *
     * @param [in] chr
     */
    String(const char chr) noexcept
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
     * @param [in] string
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(const char* string) noexcept
    {
        String str(string);
        LOG_ERROR(str.Error());
        return { str, str.Error() };
    }

    /**
     * @brief Constructs a new String object
     * from a cstring knowing its length
     *
     * @param [in] string
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(const char* string, std::size_t length) noexcept
    {
        String str(string, length);
        LOG_ERROR(str.Error());
        return { str, str.Error() };
    }

    /**
     * @brief Construct a new String object
     * and ensures that the capacity is enougth
     * for hintLength elements, thus, avoiding
     * reallocations
     *
     * @param [in] hintLength length to ensure big enough capacity
     * for less reallocations
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(std::size_t hintLength = DefaultCapacity) noexcept
    {
        String str(hintLength);
        LOG_ERROR(str.Error());
        return { str, str.Error() };
    }

    /**
     * @brief Construct a new String object
     * by copying
     *
     * @param [in] other string to copy
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(const String& other) noexcept
    {
        String str(other);
        LOG_ERROR(str.Error());
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

          char& operator[](std::size_t index)       & noexcept { return m_data[index]; }
    const char& operator[](std::size_t index) const & noexcept { return m_data[index]; }

    /**
     * @brief Returns the start of a string
     *
     * @return iterator
     */
    inline iterator      begin()        & noexcept { return RawPtr();          }

    /**
     * @brief Returns the start of a const string
     *
     * @return constIterator
     */
    inline constIterator cebgin() const & noexcept { return RawPtr();          }

    /**
     * @brief Returns the end of a string
     *
     * @return iterator
     */
    inline iterator      end()          & noexcept { return RawPtr() + length; }

    /**
     * @brief Returns the end of a const string
     *
     * @return constIterator
     */
    inline constIterator cend()   const & noexcept { return RawPtr() + length; }
///////////////////////////////////////////////////////////////////////////////
//
//                              MATH OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    bool operator==(const String& other) const noexcept
    {
        return length == other.length &&
               strcmp(RawPtr(), other.RawPtr()) == 0;
    }

    bool operator!=(const String& other) const noexcept
    {
        return !operator==(other);
    }

    String& operator+=(const char* other) noexcept
    {
        return append(other, strlen(other));
    }

    String& operator+=(const String& other) noexcept
    {
        return append(other.RawPtr(), other.length);
    }

    #define OPERATOR_PLUS_CODE                              \
    {                                                       \
        String<> result{lhs};                               \
        result += rhs;                                      \
        return result;                                      \
    }
    friend String operator+(const char* lhs, const String& rhs) noexcept
    OPERATOR_PLUS_CODE
    friend String operator+(const String& lhs, const char* rhs) noexcept
    OPERATOR_PLUS_CODE
    friend String operator+(const String& lhs, const String& rhs) noexcept
    OPERATOR_PLUS_CODE
    #undef OPERATOR_PLUS_CODE
private:
    String& append(const char* string, std::size_t strLength) noexcept
    {
        std::size_t newLength = length + strLength;

        m_data.Realloc(newLength + 1);
        if (auto err = Error())
        {
            LOG_ERROR(err);
            return *this;
        }

        std::memcpy(RawPtr() + length, string, strLength);

        length = newLength;

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
     * @return err::Result<std::size_t> index result
     */
    err::Result<std::size_t> Find(char chr) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), SIZE_MAX);

        const char* buf   = RawPtr();
        const char* found = strchr(buf, chr);
        if (!found)
            RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX);

        return found - buf;
    }

    /**
     * @brief Finds a substring and returns its index
     *
     * @param [in] string
     *
     * @return err::Result<std::size_t> index result
     */
    err::Result<std::size_t> Find(const char* string) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), SIZE_MAX);

        SoftAssertResult(string, SIZE_MAX, err::ERROR_NULLPTR);

        const char* data  = RawPtr();
        const char* found = strstr(data, string);

        if (!found)
            RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX);

        return found - data;
    }

    /**
     * @brief Finds a substring and returns its index
     *
     * @param [in] string
     *
     * @return err::Result<std::size_t> index result
     */
    err::Result<std::size_t> Find(const String& string) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), SIZE_MAX);

        const char* data  = RawPtr();
        const char* found = strstr(data, string.RawPtr());

        if (!found)
            RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX);

        return found - data;
    }

    /**
     * @brief Counts occurences of chr
     *
     * @param [in] chr char to count
     *
     * @return err::Result<std::size_t> count result
     */
    err::Result<std::size_t> Count(char chr) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), SIZE_MAX);

        std::size_t count = 0;

        for (std::size_t i = 0; i < length; i++)
            if (m_data[i] == chr)
                count++;

        return count;
    }

    /**
     * @brief Counts occurences of string
     *
     * @param [in] string string to count
     *
     * @return err::Result<std::size_t> count result
     */
    err::Result<std::size_t> Count(const char* string) const noexcept
    {
        if (!string)
            RETURN_ERROR_RESULT(err::ERROR_NULLPTR, SIZE_MAX);

        RETURN_ERROR_RESULT(Error(), SIZE_MAX);

        std::size_t count = 0;
        const char* found = strstr(RawPtr(), string);

        while (found)
        {
            count++;
            found = strstr(found + 1, string);
        }

        return count;
    }

    /**
     * @brief Counts occurences of string
     *
     * @param [in] string string to count
     *
     * @return err::Result<std::size_t> count result
     */
    err::Result<std::size_t> Count(const String& string) const noexcept
    {
        return Count(string.RawPtr());
    }
private:
    static constexpr const char* SPACE_CHARS = " \n\t\r\f\v";
public:
    /**
     * @brief Split the string by space characters
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split() const noexcept
    {
        return Split(SPACE_CHARS);
    }

    /**
     * @brief Split the string by delimeters
     *
     * @param [in] delimeters what to split by
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split(const String& delimeters) const noexcept
    {
        return Split(delimeters.RawPtr());
    }

    /**
     * @brief Split the string by delimeters
     *
     * @param [in] delimeters what to split by
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split(const char* delimiters) const noexcept
    {
        RETURN_ERROR_RESULT(Error(), {});

        char* buf = strdup(RawPtr());

        if (!buf)
            RETURN_ERROR_RESULT(err::ERROR_NO_MEMORY, {});

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
     * @return err::ErrorCode
     */
    err::ErrorCode Filter(const char* filter) noexcept
    {
        RETURN_ERROR(Error());

        SoftAssert(filter, err::ERROR_NULLPTR);

        char*       writePtr = RawPtr();
        const char* readPtr  = writePtr;

        while (*readPtr)
        {
            char c = *readPtr++;

            if (!std::strchr(filter, c))
                *writePtr++ = c;
        }

        *writePtr = '\0';

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Filters out characters
     *
     * @param [in] filter characters to filter
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Filter(const String& filter) noexcept
    {
        return Filter(filter.RawPtr());
    }

    /**
     * @brief Filters out space characters
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Filter() noexcept
    {
        return Filter(SPACE_CHARS);
    }

    void Clear() noexcept
    {
        m_data[0] = '\0';
        length = 0;
    }
};

template<>
struct Hash<String<>>
{
    uint64_t operator()(const String<>& string)
    {
        return CRC32(string.RawPtr(), string.length);
    }
};

} // namespace mlib

#endif
