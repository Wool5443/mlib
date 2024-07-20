#include <iostream>

#include "Error.hpp"

#include "Tests.hpp"

LOG_INIT_CONSOLE();

int main()
{
    std::cout << "String:\n"    << GetErrorName(Tests::TestString(100)) << "\n\n";
    std::cout << "Vector:\n"    << GetErrorName(Tests::TestVector(25))  << "\n\n";
    std::cout << "List:\n"      << GetErrorName(Tests::TestList(25))    << "\n\n";
    std::cout << "BTree:\n"     << GetErrorName(Tests::TestBTree())     << "\n\n";
    std::cout << "HashTable:\n" << GetErrorName(Tests::TestHashTable()) << "\n\n";

    LOG(err::ERROR_BAD_FIELDS);
    LOG(err::ERROR_BAD_TREE);
    LOG(err::ERROR_NULLPTR);

    return 0;
}
