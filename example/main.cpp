#include <iostream>
// #include "Tests.hpp"

#include "Tests.hpp"

int main()
{
    std::cout << "String:\n"    << GetErrorName(Tests::TestString(100)) << "\n\n";
    std::cout << "Vector:\n"    << GetErrorName(Tests::TestVector(25))  << "\n\n";
    std::cout << "List:\n"      << GetErrorName(Tests::TestList(25))    << "\n\n";
    std::cout << "BTree:\n"     << GetErrorName(Tests::TestBTree())     << "\n\n";
    std::cout << "HashTable:\n" << GetErrorName(Tests::TestHashTable()) << "\n\n";
    return 0;
}
