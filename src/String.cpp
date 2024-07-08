#include "String.hpp"

using namespace mlib;

template<size_t DefLength, size_t GrowFactor>
StringBuffer<DefLength, GrowFactor>::StringBuffer(size_t capacity, size_t length)
    : m_buf(new char[capacity]), Length(length), m_capacity(capacity) {}