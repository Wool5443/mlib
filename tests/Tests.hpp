#ifndef MLIB_TESTS_HPP
#define MLIB_TESTS_HPP

#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"
#include "BinaryTree.hpp"

namespace Tests {

Utils::Error TestString(std::size_t n);
Utils::Error TestVector(std::size_t n);
Utils::Error TestList  (std::size_t n);
Utils::Error TestBTree ();

} // namespace Test

#endif
