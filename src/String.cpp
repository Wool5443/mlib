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

std::strong_ordering CString::operator<=>(const CString& other) const noexcept
{
    int result = std::strcmp(*this, other);

    if (result < 0)
        return std::strong_ordering::less;
    else if (result == 0)
        return std::strong_ordering::equal;
    return std::strong_ordering::greater;
}
