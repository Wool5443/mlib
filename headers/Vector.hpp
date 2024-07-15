#ifndef MLIB_VECTOR_HPP
#define MLIB_VECTOR_HPP

#include "GeneralBuffer.hpp"

namespace mlib {

template<typename T, size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class Vector final
{
private:
    Buffer<T, DefaultCapacity, GrowFactor> m_buf;
public:
    size_t       Length() const noexcept  { return m_buf.Length; }
    Utils::Error Error()  const noexcept  { return m_buf.Error;  }
public:
    Vector()
        : m_buf() {}
    explicit Vector(size_t hintLength)
        : m_buf(hintLength) {}
public:
    Utils::Error PushBack(const T& element)
    {
        size_t oldLength = Length();
        RETURN_ERROR(m_buf.Realloc(oldLength + 1));

        m_buf[oldLength] = element;

        return Utils::Error();
    }
public:
    T& operator[](size_t index) & noexcept
    {
        return m_buf[index];
    }

    const T& operator[](size_t index) const & noexcept
    {
        return m_buf[index];
    }
};

} //namespace mlib

#endif
