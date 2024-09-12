#include <iostream>
#include <fstream>

#include "Tests.hpp"

// LOG_INIT_CONSOLE();

std::ofstream LOG_STREAM("../log.txt");

LOG_INIT_FILE(LOG_STREAM);

int main()
{
    const char* TEST_RESULTS[] =
    {
        GetErrorName(Tests::TestString(100)),
        GetErrorName(Tests::TestVector(25)),
        GetErrorName(Tests::TestList(25)),
        GetErrorName(Tests::TestBTree()),
        GetErrorName(Tests::TestHashTable()),
    };
    std::cout << '\n';

    std::cout << "String:    " << TEST_RESULTS[0] << "\n";
    std::cout << "Vector:    " << TEST_RESULTS[1] << "\n";
    std::cout << "List:      " << TEST_RESULTS[2] << "\n";
    std::cout << "BTree:     " << TEST_RESULTS[3] << "\n";
    std::cout << "HashTable: " << TEST_RESULTS[4] << "\n";

    RETURN_ERROR_IF(Tests::TestHashTableSpeed(10));

    LOG("Hello, friends");

    return 0;
}
