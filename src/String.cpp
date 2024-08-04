#include "String.hpp"

using namespace err;
using namespace mlib;

String::String(std::size_t hintLength) noexcept
    : m_data(hintLength + 1) {}

String::String(const char* string, std::size_t length) noexcept
    : m_data(length + 1), length(length)
{
    if (auto err = Error())
    {
        LOG_ERROR(err);
        return;
    }

    std::memcpy(RawPtr(), string, length);
}

String::String(const char* string) noexcept
    : String(string, strlen(string)) {}

String::String(const char chr) noexcept
    : String(&chr, 1) {}

char& String::operator[](std::size_t index) & noexcept
{
    return m_data[index];
}

const char& String::operator[](std::size_t index) const & noexcept
{
    return m_data[index];
}

String::iterator String::begin() & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr();
}

String::constIterator String::begin() const & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr();
}

String::constIterator String::cbegin() const & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr();
}

String::iterator String::end() & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr() + length;
}

String::constIterator String::end() const & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr() + length;
}

String::constIterator String::cend() const & noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return nullptr;
    }

    return m_data.RawPtr() + length;
}

std::strong_ordering String::operator<=>(const char* other) const noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return std::strong_ordering::greater;
    }

    int result = std::strcmp(*this, other);

    if (result < 0)
        return std::strong_ordering::less;
    else if (result == 0)
        return std::strong_ordering::equal;
    return std::strong_ordering::greater;
}

bool String::operator==(const String& other) const noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return false;
    }
    if (auto error = other.Error())
    {
        LOG_ERROR(error);
        return false;
    }

    return length == other.length &&
           strcmp(RawPtr(), other.RawPtr()) == 0;
}

bool String::operator==(const char* other) const noexcept
{
    if (auto error = Error())
    {
        LOG_ERROR(error);
        return false;
    }

    if (!other)
        return false;

    return strcmp(RawPtr(), other) == 0;
}


bool String::operator!=(const String& other) const noexcept
{
    return !operator==(other);
}

bool String::operator!=(const char* other) const noexcept
{
    return !operator==(other);
}

String& String::append(const char* string, std::size_t strLength) noexcept
{
    if (auto error = Error())
    {
        if (error != ERROR_UNINITIALIZED)
        {
            LOG_ERROR(error);
            return *this;
        }
    }

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

String& String::operator+=(const char* other) noexcept
{
    return append(other, strlen(other));
}

String& String::operator+=(const String& other) noexcept
{
    return append(other.RawPtr(), other.length);
}

#define OPERATOR_PLUS_CODE                              \
{                                                       \
    String result{lhs};                                 \
    LOG_ERROR_IF(result.Error());                       \
    result += rhs;                                      \
    return result;                                      \
}

#define RVAL_OPERATOR_PLUS_CODE                         \
{                                                       \
    lhs += rhs;                                         \
    return lhs;                                         \
}
namespace mlib {

String operator+(const char* lhs, const String& rhs) noexcept
OPERATOR_PLUS_CODE
/**
 * @brief Adds s2 strings
 */
String operator+(const String& lhs, const char* rhs) noexcept
OPERATOR_PLUS_CODE
/**
 * @brief Adds s2 strings
 */
String operator+(const String& lhs, const String& rhs) noexcept
OPERATOR_PLUS_CODE
/**
 * @brief Adds s2 strings
 */
String operator+(String&& lhs, const char* rhs) noexcept
RVAL_OPERATOR_PLUS_CODE
/**
 * @brief Adds s2 strings
 */
String operator+(String&& lhs, const String& rhs) noexcept
RVAL_OPERATOR_PLUS_CODE

std::ostream& operator<<(std::ostream& out, const String& string)
{
    if (auto error = string.Error())
    {
        LOG_ERROR(error);
        return out;
    }
    return out << string.RawPtr();
}

} // namespace mlib

#undef OPERATOR_PLUS_CODE
#undef RVAL_OPERATOR_PLUS_CODE

String::operator char*()                & noexcept { return RawPtr(); }
String::operator const char*()    const & noexcept { return RawPtr(); }
String::operator bool()           const   noexcept { return Error();  }

Result<String> String::ReadFromFile(const char* filePath) noexcept
{
    HardAssert(filePath, ERROR_NULLPTR);

    struct stat result = {};
    stat(filePath, &result);

    std::ifstream input(filePath);
    if (!input)
        return ERROR_BAD_FILE;

    String str(static_cast<std::size_t>(result.st_size));
    RETURN_ERROR_RESULT(str.Error(), {}, String);

    input.read(str, result.st_size);

    str.length = result.st_size;

    return str;
}

Result<std::size_t> String::Find(char chr) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX, std::size_t);

    const char* buf   = RawPtr();
    const char* found = strchr(buf, chr);
    if (!found)
        RETURN_ERROR_RESULT(ERROR_NOT_FOUND, SIZE_MAX, std::size_t);

    return found - buf;
}

Result<std::size_t> String::Find(const char* string) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX, std::size_t);

    SoftAssertResult(string, SIZE_MAX, ERROR_NULLPTR);

    const char* data  = RawPtr();
    const char* found = strstr(data, string);

    if (!found)
        RETURN_ERROR_RESULT(ERROR_NOT_FOUND, SIZE_MAX, std::size_t);

    return found - data;
}

Result<std::size_t> String::Count(char chr) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX, std::size_t);

    std::size_t count = 0;

    for (std::size_t i = 0; i < length; i++)
        if (m_data[i] == chr)
            count++;

    return count;
}

Result<std::size_t> String::Count(const char* string) const noexcept
{
    if (!string)
        RETURN_ERROR_RESULT(ERROR_NULLPTR, SIZE_MAX, std::size_t);

    RETURN_ERROR_RESULT(Error(), SIZE_MAX, std::size_t);

    std::size_t count = 0;
    const char* found = strstr(RawPtr(), string);

    while (found)
    {
        count++;
        found = strstr(found + 1, string);
    }

    return count;
}

Result<Vector<String>> String::Split(const char* delimiters) const noexcept
{
    RETURN_ERROR_RESULT(Error(), {}, Vector<String>);

    char* buf = strdup(RawPtr());

    if (!buf)
        RETURN_ERROR_RESULT(ERROR_NO_MEMORY, {}, Vector<String>);

    Vector<String> words;

    const char* token = strtok(buf, delimiters);

    std::size_t i = 0;

    while (token)
    {
        RETURN_ERROR_RESULT(words.PushBack(token), {}, Vector<String>, free(buf));
        token = strtok(nullptr, delimiters);
    }

    free(buf);

    return words;
}

ErrorCode String::Filter(const char* filter) noexcept
{
    RETURN_ERROR(Error());

    SoftAssert(filter, ERROR_NULLPTR);

    char*       writePtr = RawPtr();
    const char* readPtr  = writePtr;

    while (*readPtr)
    {
        char c = *readPtr++;

        if (!std::strchr(filter, c))
            *writePtr++ = c;
    }

    *writePtr = '\0';

    return EVERYTHING_FINE;
}

ErrorCode String::Filter(const String& filter) noexcept
{
    return Filter(filter.RawPtr());
}

ErrorCode String::Filter() noexcept
{
    return Filter(SPACE_CHARS);
}

ErrorCode String::Clear() noexcept
{
    RETURN_ERROR(Error());

    m_data[0] = '\0';
    length    = 0;

    return EVERYTHING_FINE;
}

Result<Vector<CString>> mlib::Split(const CString string, const CString delimiters) noexcept
{
    Vector<CString> words;

    CString tempStr  = string;
    CString nextWord = tempStr.getNextWord(delimiters);

    while (nextWord)
    {
        RETURN_ERROR_RESULT(words.PushBack(nextWord), {}, Vector<CString>);
        nextWord = tempStr.getNextWord(delimiters);
    }

    return words;
}
