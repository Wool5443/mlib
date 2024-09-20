#include <iostream>
#include <string>
#include "HashTable.hpp"

#include "Tests.hpp"

using namespace mlib;
using namespace err;

LOG_INIT_CONSOLE();
// LOG_INIT_FILE("../log.txt");

int main()
{
    Timer time;
    std::cout << GetErrorName(Tests::TestHashTable()) << std::endl;
    time.PrintDuration(std::cout);

    return 0;
}
