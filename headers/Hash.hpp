#ifndef MLIB_HASH_HPP
#define MLIB_HASH_HPP

#include <stdint.h>

#ifdef __linux__
extern "C" uint64_t CRC32(const void* data, size_t size, uint64_t seed = 0xBEBDA);
#endif
extern "C" uint64_t MurMur(const void* data, size_t size, uint64_t seed = 0xBEBDA);

namespace mlib {

using HashType = uint64_t;

template<typename T>
struct Hash
{
    HashType operator()()
    {
        return 0;
    }
};

} // namespace mlin

#endif
