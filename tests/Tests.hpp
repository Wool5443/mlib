#ifndef MLIB_TESTS_HPP
#define MLIB_TESTS_HPP

#include "Utils.hpp"

namespace Tests {

Utils::Error TestString(std::size_t n);
Utils::Error TestVector(std::size_t n);
Utils::Error TestList  (std::size_t n);
Utils::Error TestBTree ();

} // namespace Test

#endif
