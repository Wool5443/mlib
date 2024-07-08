#ifndef MLIB_LINKED_LIST_HPP
#define MLIB_LINKED_LIST_HPP

#include "GeneralBuffer.hpp"

namespace mlib {

template<typename T, size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class LinkedList
{
private:
    Buffer<T,      DefaultCapacity, GrowFactor> m_data;
    Buffer<size_t, DefaultCapacity, GrowFactor> m_next;
    Buffer<size_t, DefaultCapacity, GrowFactor> m_prev;
public:
    size_t       Length() const noexcept { return m_data.Length; }
    Utils::Error Error()  const noexcept { return m_data.Error;  }
public:
    LinkedList()
        : LinkedList(0) {}

    LinkedList(size_t hintLength)
        : m_data(hintLength), m_next(hintLength), m_prev(hintLength) {}

          T& operator[](size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](size_t index) const & noexcept { return m_data[index]; }
};

} // namespace mlib

#endif
