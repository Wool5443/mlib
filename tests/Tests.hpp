#ifndef MLIB_TESTS_HPP
#define MLIB_TESTS_HPP

#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"
#include "BinaryTree.hpp"

namespace Tests {

Utils::Error TestString(size_t n);
Utils::Error TestVector(size_t n);
Utils::Error TestList  (size_t n);
Utils::Error TestBTree ();

} // namespace Test

#endif
