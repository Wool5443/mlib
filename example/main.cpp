#include <iostream>
#include <fstream>

#include "Tests.hpp"

// LOG_INIT_CONSOLE();

LOG_INIT_FILE("../log.txt");

int main()
{
    char* buf = mlib::ReadFileToBuf("../tests/Words.txt");
    std::cout << buf << std::endl;
    delete[] buf;
    #if 0
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
    #endif

    LOG("Hello, friends");

    return 0;
}
