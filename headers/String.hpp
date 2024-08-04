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
#include <fstream>
#include <compare>
#include <cstring>
#include <sys/stat.h>
#include "Hash.hpp"
#include "Error.hpp"
#include "Vector.hpp"

namespace mlib {

constexpr const char* SPACE_CHARS = " \n\t\r\f\v";

/** @struct CString
 * @brief Represents a simple static string
 * with some useful methods and operators
 */
class CString final
{
    const char* m_data   = nullptr;
    std::size_t m_length = 0;
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Return pointer to its data
     *
     * @return const char*
     */
    constexpr const char* RawPtr() const noexcept { return m_data;   }

    /**
     * @brief Length
     *
     * @return std::size_t
     */
    constexpr std::size_t Length() const noexcept { return m_length; }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new CString object
     */
    constexpr CString() noexcept = default;

    /**
     * @brief Construct a new CString object
     * from a c-style string
     *
     * @param [in] string
     */
    constexpr CString(const char* string) noexcept
        : m_data(string), m_length(strlen(string)) {}

    /**
     * @brief Construct a new CString object
     *
     * @param [in] string
     * @param [in] length
     */
    constexpr CString(const char* string, std::size_t length) noexcept
        : m_data(string), m_length(length) {}
///////////////////////////////////////////////////////////////////////////////
//
//                              OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief operator[]
     *
     * @param i
     * @return constexpr char
     */
    constexpr char operator[](std::size_t i) const noexcept { return m_data[i]; }

    /**
     * @brief Careful, the string may be not null-terminated
     *
     * @return cconst char*
     */
    constexpr operator const char*() const noexcept { return m_data; }
    constexpr operator bool()        const noexcept { return m_data; }

    /**
     * @brief Stream print
     *
     * @param out stream
     * @param cstring string
     * @return std::ostream&
     */
    friend std::ostream& operator<<(std::ostream& out, const CString& string)
    {
        for (std::size_t i = 0; i < string.Length(); i++)
            out << string[i];
        return out;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              COMPARISON
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief CString comparator
     *
     * @param [in] other
     *
     * @return std::strong_ordering
     */
    constexpr std::strong_ordering operator<=>(const CString& other) const noexcept
    {
        int result = std::memcmp(
            m_data, other.m_data, std::min(m_length, other.m_length)
        );

        if (result < 0)
            return std::strong_ordering::less;
        else if (result == 0)
            return std::strong_ordering::equal;
        return std::strong_ordering::greater;
    }

    /**
     * @brief
     *
     * @param [in] other
     *
     * @return true equal
     * @return false not equal
     */
    constexpr bool operator==(const CString& other) const noexcept
    {
        return *this <=> other == std::strong_ordering::equal;
    }
    constexpr bool operator==(const char* other) const noexcept
    {
        return *this == CString(other);
    }

    friend err::Result<Vector<CString>>
    Split
    (const CString str, const CString delimiters) noexcept;
private:
    constexpr CString getNextWord(const CString& delimiters) noexcept
    {
        std::size_t i;
        for (i = 0; i < m_length; i++)
        {
            for (std::size_t di = 0; di < delimiters.m_length; di++)
            {
                if (m_data[i] == delimiters[di])
                {
                    CString result{m_data, i};
                    m_length -= i + 1;
                    m_data   += i + 1;
                    return result;
                }
            }
        }

        // last word
        CString result{m_data, i};
        m_length = 0;
        m_data   = nullptr;
        return result;
    }
};

/** @class String
 * @brief A resizeable string
 *
 * @tparam DefaultCapacity
 * @tparam GrowFactor
 */
class String final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    Buffer<char> m_data{};
public:
    std::size_t  length = 0; ///< string length
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
    constexpr String() noexcept = default;

    /**
     * @brief Construct a String with set capacity
     *
     * @param [in] capacity
     */
    explicit String(std::size_t capcity) noexcept;

    /**
     * @brief Construct a new String object
     * from a c-style string knowing its length
     *
     * @param [in] string
     * @param [in] length
     */
    String(const char* string, std::size_t length) noexcept;

    /**
     * @brief Construct a new String object
     * from a c-style string
     *
     * @param [in] string
     */
    String(const char* string) noexcept;

    /**
     * @brief Construct a new String object
     * from a character
     *
     * @param [in] chr
     */
    String(const char chr) noexcept;
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Reads a file and puts it into a string
     *
     * @param [in] filePath path to file to read
     *
     * @return err::Result<String>
     */
    static err::Result<String> ReadFromFile(const char* filePath) noexcept;

    /**
     * @brief Finds a char and returns its index
     *
     * @param [in] chr char to find
     *
     * @return err::Result<std::size_t> index result
     */
    err::Result<std::size_t> Find(char chr) const noexcept;

    /**
     * @brief Finds a substring and returns its index
     *
     * @param [in] string
     *
     * @return err::Result<std::size_t> index result
     */
    err::Result<std::size_t> Find(const char* string) const noexcept;

    /**
     * @brief Counts occurences of chr
     *
     * @param [in] chr char to count
     *
     * @return err::Result<std::size_t> count result
     */
    err::Result<std::size_t> Count(char chr) const noexcept;

    /**
     * @brief Counts occurences of string
     *
     * @param [in] string string to count
     *
     * @return err::Result<std::size_t> count result
     */
    err::Result<std::size_t> Count(const char* string) const noexcept;
public:
    /**
     * @brief Split the string by delimeters
     *
     * @param [in] delimeters what to split by or SPACE_CHARS
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split(const char* delimiters = SPACE_CHARS)
    const noexcept;

    /**
     * @brief Filters out characters
     *
     * @param [in] filter characters to filter
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Filter(const char* filter) noexcept;

    /**
     * @brief Filters out characters
     *
     * @param [in] filter characters to filter
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Filter(const String& filter) noexcept;

    /**
     * @brief Filters out space characters
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Filter() noexcept;

    /**
     * @brief Clears the string
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Clear() noexcept;
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    using iterator      = char*;
    using constIterator = const char*;

          char& operator[](std::size_t index)       & noexcept;
    const char& operator[](std::size_t index) const & noexcept;

    /**
     * @brief Returns the start of a buffer
     *
     * @return iterator
     */
    iterator      begin()        & noexcept;

    /**
     * @brief Returns the start of a const buffer
     *
     * @return constIterator
     */
    constIterator begin()  const & noexcept;

    /**
     * @brief Returns the start of a const buffer
     *
     * @return constIterator
     */
    constIterator cbegin() const & noexcept;

    /**
     * @brief Returns the end of a buffer
     *
     * @return iterator
     */
     iterator     end()          & noexcept;

    /**
     * @brief Returns the end of a const buffer
     *
     * @return constIterator
     */
    constIterator end()    const & noexcept;

    /**
     * @brief Returns the end of a const buffer
     *
     * @return constIterator
     */
    constIterator cend()   const & noexcept;
///////////////////////////////////////////////////////////////////////////////
//
//                              MATH OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
private:
    String& append(const char* string, std::size_t strLength) noexcept;
public:
    /**
     * @brief Operator spaceship comparison
     *
     * @param [in] other to compare
     *
     * @return std::strong_ordering
     */
    std::strong_ordering operator<=>(const char* other) const noexcept;

    /**
     * @brief Compares string for equality
     *
     * @param [in] other
     *
     * @return true equal
     * @return false not equal
     */
    bool operator==(const String& other) const noexcept;
    bool operator==(const char* other)   const noexcept;

    /**
     * @brief Compares string for inequality
     *
     * @param [in] other
     *
     * @return true not equal
     * @return false equal
     */
    bool operator!=(const String& other) const noexcept;
    bool operator!=(const char* other)   const noexcept;

    /**
     * @brief appends other
     *
     * @param [in] other string to append
     *
     * @return String& result
     */
    String& operator+=(const char* other) noexcept;
    String& operator+=(const String& other) noexcept;

    /**
     * @brief Adds s2 strings
     */
    friend String operator+(const char* lhs, const String& rhs) noexcept;

    /**
     * @brief Adds s2 strings
     */
    friend String operator+(const String& lhs, const char* rhs) noexcept;

    /**
     * @brief Adds s2 strings
     */
    friend String operator+(const String& lhs, const String& rhs) noexcept;

    /**
     * @brief Adds s2 strings
     */
    friend String operator+(String&& lhs, const char* rhs) noexcept;

    /**
     * @brief Adds s2 strings
     */
    friend String operator+(String&& lhs, const String& rhs) noexcept;
///////////////////////////////////////////////////////////////////////////////
//
//                              STREAM OPERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Prints string to std::ostream
     *
     * @param [in] out
     * @param [in] string
     *
     * @return std::ostream&
     */
    friend std::ostream& operator<<(std::ostream& out, const String& string);
///////////////////////////////////////////////////////////////////////////////
//
//                              IMPLICIT CASTS
//
///////////////////////////////////////////////////////////////////////////////
public:
    operator char*()                & noexcept;
    operator const char*()    const & noexcept;
    operator bool()           const   noexcept;
};

template<>
struct Hash<CString>
{
    HashType operator()(const CString& cstring)
    {
        #ifdef __linux__
        return CRC32(cstring.RawPtr(), cstring.Length());
        #else
        return MurMur(cstring.data, cstring.length);
        #endif
    }
};

template<>
struct Hash<String>
{
    HashType operator()(const String& string)
    {
        #ifdef __linux__
        return CRC32(string.RawPtr(), string.length);
        #else
        return MurMur(string.RawPtr(), string.length);
        #endif
    }
};

err::Result<Vector<CString>>
Split(const CString string, const CString delimiters = SPACE_CHARS) noexcept;

} // namespace mlib

#endif
