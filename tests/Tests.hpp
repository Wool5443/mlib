#ifndef MLIB_TESTS_HPP
#define MLIB_TESTS_HPP

#include "Error.hpp"

namespace Tests {

err::ErrorCode TestString   (std::size_t n);
err::ErrorCode TestVector   (std::size_t n);
err::ErrorCode TestList     (std::size_t n);
err::ErrorCode TestBTree    ();
err::ErrorCode TestHashTable();
err::ErrorCode TestHashTableSpeed(std::size_t numberOfTests);

} // namespace Test

#endif
