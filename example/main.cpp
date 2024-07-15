#include "Tests.hpp"

int main()
{
    Tests::TestString(100).Print();
    Tests::TestVector(25).Print();
    Tests::TestList(25).Print();
    Tests::TestBTree(25).Print();
    return 0;
}
