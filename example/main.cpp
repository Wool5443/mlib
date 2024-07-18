#include "Tests.hpp"

int main()
{
    std::cout << "String: " << Tests::TestString(100).GetErrorName() << '\n';
    std::cout << "Vector: " << Tests::TestVector(25).GetErrorName() << '\n';
    std::cout << "List:   " << Tests::TestList(25).GetErrorName() << '\n';
    std::cout << "BTree:  " << Tests::TestBTree().GetErrorName() << '\n';
    return 0;
}
