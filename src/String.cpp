#include "String.hpp"

mlib::CString::CString(const char* string) noexcept
    : data(string), length(strlen(string)) {}

mlib::CString::CString(const char* string, std::size_t length) noexcept
    : data(string), length(length) {}

mlib::CString::operator const char*() const noexcept
{
    return data;
}

mlib::CString::operator bool() const noexcept
{
    return data;
}

std::strong_ordering mlib::CString::operator<=>(const mlib::CString& other) const noexcept
{
    int result = std::strcmp(this->data, other.data);

    if (result < 0)
        return std::strong_ordering::less;
    else if (result == 0)
        return std::strong_ordering::equal;
    return std::strong_ordering::greater;
}

bool mlib::CString::operator==(const CString& other) const noexcept
{
    return *this <=> other == std::strong_ordering::equal;
}
