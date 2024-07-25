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
    String() noexcept = default;

    /**
     * @brief Construct a new String object
     * with known length
     *
     * @param [in] hintLength legnth to ensure capacity
     * for less reallocations
     */
    explicit String(std::size_t hintLength) noexcept;

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
    static err::Result<String> New(const char* string) noexcept;

    /**
     * @brief Constructs a new String object
     * from a cstring knowing its length
     *
     * @param [in] string
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(const char* string, std::size_t length)
    noexcept;

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
    static err::Result<String> New(std::size_t hintLength) noexcept;

    /**
     * @brief Construct a new String object
     * by copying
     *
     * @param [in] other string to copy
     *
     * @return err::Result<String>
     */
    static err::Result<String> New(const String& other) noexcept;
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
private:
    constexpr static const char* SPACE_CHARS = " \n\t\r\f\v";
public:
    /**
     * @brief Split the string by space characters
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split() const noexcept;

    /**
     * @brief Split the string by delimeters
     *
     * @param [in] delimeters what to split by
     *
     * @return err::Result<Vector<String>> vector of strings
     */
    err::Result<Vector<String>> Split(const char* delimiters) const noexcept;

    /**
     * @brief Splits the string in place by space chars.
     * It's more memory and speed efficient, though,
     * modifies the string
     *
     * @param [in] string
     *
     * @return err::Result<Vector<const char*>>
     */
    static err::Result<Vector<const char*>>
    SplitInPlace(char* string) noexcept;

    /**
     * @brief Splits the string in place.
     * It's more memory and speed efficient, though,
     * modifies the string
     *
     * @param [in] string
     * @param [in] delimiters what to split by
     *
     * @return err::Result<Vector<const char*>>
     */
    static err::Result<Vector<const char*>>
    SplitInPlace(char* string, const char* delimiters) noexcept;

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

/** @struct CString
 * @brief Represents a simple static string
 * with some useful methods and operators
 */
struct CString final
{
    const char* data   = nullptr; ///< data null-terminated string
    std::size_t length = 0; ///< length

    /**
     * @brief Construct a new CString object
     */
    CString() noexcept = default;

    /**
     * @brief Construct a new CString object
     *
     * @param [in] string
     */
    CString(const char* string) noexcept;

    /**
     * @brief Construct a new CString object
     *
     * @param [in] string
     * @param [in] length
     */
    CString(const char* string, std::size_t length) noexcept;

    /**
     * @brief CString comparator
     *
     * @param [in] other
     *
     * @return std::strong_ordering
     */
    std::strong_ordering operator<=>(const CString& other) const noexcept;

    /**
     * @brief
     *
     * @param [in] other
     *
     * @return true equal
     * @return false not equal
     */
    bool operator==(const CString& other) const noexcept;

    operator const char*() const noexcept;
    operator bool()        const noexcept;

    friend std::ostream& operator<<(std::ostream& out, const CString& cstring);
};

template<>
struct Hash<CString>
{
    HashType operator()(const CString& cstring)
    {
        return CRC32(cstring.data, cstring.length);
    }
};

template<>
struct Hash<String>
{
    HashType operator()(const String& string)
    {
        return CRC32(string.RawPtr(), string.length);
    }
};

} // namespace mlib

#endif
