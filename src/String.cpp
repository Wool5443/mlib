#include "String.hpp"

using namespace err;
using namespace mlib;

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


namespace mlib {

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
    RETURN_ERROR_IF(Error());

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

ErrorCode String::Clear() noexcept
{
    RETURN_ERROR_IF(Error());

    m_data[0] = '\0';
    length    = 0;

    return EVERYTHING_FINE;
}

Result<Vector<str>> mlib::Split(const str string, const str delimiters) noexcept
{
    Vector<str> words;

    str tempStr  = string;
    str nextWord = tempStr.getNextWord(delimiters);

    while (nextWord)
    {
        RETURN_ERROR_RESULT(words.PushBack(nextWord), {}, Vector<str>);
        nextWord = tempStr.getNextWord(delimiters);
    }

    return words;
}
