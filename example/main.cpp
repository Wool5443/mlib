#include <iostream>
#include "Tests.hpp"

int main()
{
    std::cout << "String:\n" << Tests::TestString(100).GetErrorName() << "\n\n";
    std::cout << "Vector:\n" << Tests::TestVector(25).GetErrorName() << "\n\n";
    std::cout << "List:\n" << Tests::TestList(25).GetErrorName() << "\n\n";
    std::cout << "BTree:\n" << Tests::TestBTree().GetErrorName() << "\n\n";
    std::cout << "HashTable:\n" << Tests::TestHashTable().GetErrorName() << "\n\n";
    return 0;
}
