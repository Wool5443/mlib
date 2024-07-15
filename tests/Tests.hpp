#ifndef MLIB_TESTS_HPP
#define MLIB_TESTS_HPP

#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"

namespace Tests
{
Utils::Error TestString();
Utils::Error TestVector(size_t size);
Utils::Error TestList  (size_t size);
} // namespace Test

#endif
