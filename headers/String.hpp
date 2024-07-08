#ifndef MY_STLIB_STRING_HPP
#define MY_STLIB_STRING_HPP

#include <cstddef>
#include "Utils.hpp"

namespace mlib
{
template<size_t DefLength, size_t GrowFactor>
class StringBuffer
{
private:
    char*  m_buf;
    size_t m_capacity;

    StringBuffer(size_t capacity, size_t length);
public:
    size_t       Length;
    Utils::Error error = Utils::Error();
};
}

#endif
