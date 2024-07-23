#include <iostream>
#include <fstream>

#include <string>
#include <unordered_set>

#include "Tests.hpp"

// LOG_INIT_CONSOLE();

std::ofstream LOG_STREAM("../log.txt");

LOG_INIT_FILE(LOG_STREAM);

int main()
{
    std::cout << "String:\n"    << GetErrorName(Tests::TestString(100)) << "\n\n";
    std::cout << "Vector:\n"    << GetErrorName(Tests::TestVector(25))  << "\n\n";
    std::cout << "List:\n"      << GetErrorName(Tests::TestList(25))    << "\n\n";
    std::cout << "BTree:\n"     << GetErrorName(Tests::TestBTree())     << "\n\n";
    std::cout << "HashTable:\n" << GetErrorName(Tests::TestHashTable()) << "\n\n";

    LOG("Hello, friends");

    return 0;
}
