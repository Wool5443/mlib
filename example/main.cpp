#include <iostream>
#include <string>
#include "HashTable.hpp"

#include "Tests.hpp"

using namespace mlib;
using namespace err;

// LOG_INIT_CONSOLE();

LOG_INIT_FILE("../log.txt");

int main()
{
    const char* TEST_RESULTS[] =
    {
    //     // GetErrorName(Tests::TestString(100)),
    //     // GetErrorName(Tests::TestVector(25)),
    //     // GetErrorName(Tests::TestList(25)),
    //     // GetErrorName(Tests::TestBTree()),
        GetErrorName(Tests::TestHashTable()),
    };
    std::cout << '\n';

    // // std::cout << "String:    " << TEST_RESULTS[0] << "\n";
    // // std::cout << "Vector:    " << TEST_RESULTS[1] << "\n";
    // // std::cout << "List:      " << TEST_RESULTS[2] << "\n";
    // // std::cout << "BTree:     " << TEST_RESULTS[3] << "\n";
    std::cout << "HashTable: " << TEST_RESULTS[0] << "\n";

    // RETURN_ERROR_IF(Tests::TestHashTableSpeed(10));

    HashTable<std::string_view, int> tab;

    for (std::size_t i = 0; i < 50; i++)
    {
        char c = 'a' + i;
        std::string s{c};

        tab.Insert(s, i);
    }

    for (std::size_t i = 0; i < 50; i++)
    {
        char c = 'a' + i;
        std::string s{c};

        std::cout << tab.Find(s)->value << std::endl;
    }

    return 0;
}
