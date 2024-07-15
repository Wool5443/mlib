#include "Tests.hpp"

int main()
{
    Tests::TestString(100).Print();
    Tests::TestVector(25).Print();
    Tests::TestList(25).Print();
    Tests::TestBTree().Print();
    return 0;
}
