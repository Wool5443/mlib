#include "String.hpp"

using namespace mlib;

CString::CString(const char* string) noexcept
    : data(string), length(strlen(string)) {}

CString::CString(const char* string, std::size_t length) noexcept
    : data(string), length(length) {}

CString::operator const char*() const noexcept
{
    return data;
}

CString::operator bool() const noexcept
{
    return data;
}

std::strong_ordering CString::operator<=>(const mlib::CString& other) const noexcept
{
    int result = std::strcmp(this->data, other.data);

    if (result < 0)
        return std::strong_ordering::less;
    else if (result == 0)
        return std::strong_ordering::equal;
    return std::strong_ordering::greater;
}

bool CString::operator==(const CString& other) const noexcept
{
    return *this <=> other == std::strong_ordering::equal;
}

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


err::Result<String> String::New(const char* string) noexcept
{
    String str(string);
    LOG_ERROR_IF(str.Error());
    return { str, str.Error() };
}

err::Result<String> String::New(const char* string, std::size_t length) noexcept
{
    String str(string, length);
    LOG_ERROR_IF(str.Error());
    return { str, str.Error() };
}

err::Result<String> String::New(std::size_t hintLength) noexcept
{
    String str(hintLength);
    LOG_ERROR_IF(str.Error());
    return { str, str.Error() };
}

err::Result<String> String::New(const String& other) noexcept
{
    String str(other);
    LOG_ERROR_IF(str.Error());
    return { str, str.Error() };
}

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
        if (error != err::ERROR_UNINITIALIZED)
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

err::Result<String> String::ReadFromFile(const char* filePath) noexcept
{
    HardAssert(filePath, err::ERROR_NULLPTR);

    struct stat result = {};
    stat(filePath, &result);

    std::ifstream input(filePath);
    if (!input)
        return err::ERROR_BAD_FILE;

    auto strRes = String::New(result.st_size);
    RETURN_RESULT(strRes);

    input.read(strRes.value, result.st_size);

    strRes.value.length = result.st_size;

    return strRes;
}

err::Result<std::size_t> String::Find(char chr) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX);

    const char* buf   = RawPtr();
    const char* found = strchr(buf, chr);
    if (!found)
        RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX);

    return found - buf;
}

err::Result<std::size_t> String::Find(const char* string) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX);

    SoftAssertResult(string, SIZE_MAX, err::ERROR_NULLPTR);

    const char* data  = RawPtr();
    const char* found = strstr(data, string);

    if (!found)
        RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX);

    return found - data;
}

err::Result<std::size_t> String::Count(char chr) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX);

    std::size_t count = 0;

    for (std::size_t i = 0; i < length; i++)
        if (m_data[i] == chr)
            count++;

    return count;
}

err::Result<std::size_t> String::Count(const char* string) const noexcept
{
    RETURN_ERROR_RESULT(Error(), SIZE_MAX);

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

err::Result<Vector<String>> String::Split() const noexcept
{
    return Split(SPACE_CHARS);
}

err::Result<Vector<String>> String::Split(const char* delimiters) const noexcept
{
    RETURN_ERROR_RESULT(Error(), {});

    char* buf = strdup(RawPtr());

    if (!buf)
        RETURN_ERROR_RESULT(err::ERROR_NO_MEMORY, {});

    Vector<String> words;

    const char* token = strtok(buf, delimiters);

    std::size_t i = 0;

    while (token)
    {
        RETURN_ERROR_RESULT(words.PushBack(token), {}, free(buf));
        token = strtok(nullptr, delimiters);
    }

    free(buf);

    return words;
}

err::Result<Vector<const char*>>
String::SplitInPlace(char* string) noexcept
{
    return SplitInPlace(string, SPACE_CHARS);
}

err::Result<Vector<const char*>>
String::SplitInPlace(char* string, const char* delimiters) noexcept
{
    HardAssert(string,     err::ERROR_NULLPTR);
    HardAssert(delimiters, err::ERROR_NULLPTR);

    Vector<const char*> words;

    const char* token = strtok(string, delimiters);

    while (token)
    {
        words.PushBack(token);
        token = strtok(nullptr, delimiters);
    }

    return words;
}

err::ErrorCode String::Filter(const char* filter) noexcept
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

err::ErrorCode String::Filter(const String& filter) noexcept
{
    return Filter(filter.RawPtr());
}

err::ErrorCode String::Filter() noexcept
{
    return Filter(SPACE_CHARS);
}

err::ErrorCode String::Clear() noexcept
{
    RETURN_ERROR(Error());

    m_data[0] = '\0';
    length    = 0;

    return err::EVERYTHING_FINE;
}