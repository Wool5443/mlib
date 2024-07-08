#ifndef MLIB_VECTOR_HPP
#define MLIB_VECTOR_HPP

#include "GeneralBuffer.hpp"

namespace mlib {

template<typename T, size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class Vector final
{
    Buffer<T, DefaultCapacity, GrowFactor> m_buf;

public:
    Vector()
        : m_buf() {}
    Vector(size_t hintLength)
        : m_buf(hintLength) {}
};

} //namespace mlib

#endif
