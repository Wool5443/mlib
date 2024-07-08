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
    Vector(size_t hintLength)
        : m_buf(hintLength) {}
public:
    void PushBack(const T& element)
    {
        size_t newLength = Length() + 1;

        m_buf.Realloc(newLength);
        if (Error()) return;

        m_buf[Length()] = element;

        m_buf.Length++;
    }

    void PushBack(T&& element)
    {

        size_t newLength = Length() + 1;

        m_buf.Realloc(newLength);
        if (Error()) return;

        m_buf[Length()] = element;

        m_buf.Length++;
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
