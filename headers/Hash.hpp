#pragma once

#include "Utils.hpp"

extern "C" uint64_t CRC32(const void* data, size_t size, uint64_t seed);

namespace mlib {

template<typename T>
struct CRC32Functor
{
    uint64_t operator()(const T& object)
    {
        return CRC32(object.RawPtr(), object.length, 0);
    }

    uint64_t operator()(T&& object)
    {
        return CRC32(object.RawPtr(), object.length, 0);
    }
};

} // namespace mlin