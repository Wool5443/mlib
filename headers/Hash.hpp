#ifndef MLIB_HASH_HPP
#define MLIB_HASH_HPP

#include <stdint.h>

extern "C" uint64_t CRC32(const void* data, size_t size, uint64_t seed);

namespace mlib {

template<typename T>
struct CRC32Functor
{
    uint64_t operator()(const T& object) const noexcept
    {
        return CRC32(object.RawPtr(), object.length, 0);
    }

    uint64_t operator()(T&& object) const noexcept
    {
        return CRC32(object.RawPtr(), object.length, 0);
    }
};

} // namespace mlin

#endif
